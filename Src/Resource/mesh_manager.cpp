#include "mesh_manager.h"

#include <fstream>

#include "mesh_file.h"
#include "vertex_types.h"

void MeshManager::Initialize(ID3D12Device* device)
{
    device_ = device;
}

void MeshManager::Shutdown()
{
    cache_.clear();
    device_ = nullptr;
}

Mesh* MeshManager::Load(const std::string& path)
{
    if (device_ == nullptr)
    {
        return nullptr;
    }

    if (auto it = cache_.find(path); it != cache_.end())
    {
        return it->second.get();
    }

    std::ifstream ifs(path, std::ios::binary);
    if (!ifs)
    {
        MessageBox(nullptr, L"Failed to read mesh file", L"Error", MB_OK);
        return nullptr;
    }

    MeshFileHeader header = {};
    ifs.read(reinterpret_cast<char*>(&header), sizeof(MeshFileHeader));
    if (!ifs)
    {
        MessageBox(nullptr, L"Failed to read file", L"Error", MB_OK);
        return nullptr;
    }
    if (std::memcmp(header.magic, kMeshMagic, 4) != 0)
    {
        MessageBoxA(nullptr, "Invalid magic", "Error", MB_OK);
        return nullptr;
    }

    if (header.version != kMeshVersion)
    {
        char msg[256];
        sprintf_s(msg, "Invalid version mesh %s", path);
        // filename ‚Í const char*                          
        MessageBoxA(nullptr, msg, "Error", MB_OK);
        return nullptr;
    }

    if (header.vertex_stride != sizeof(StaticVertex))
    {
        MessageBoxA(nullptr, "Invalid vertex stride", "Error", MB_OK);
        return nullptr;
    }

    if (header.index_stride != sizeof(uint32_t))
    {
        MessageBoxA(nullptr, "Invalid index stride", "Error", MB_OK);
        return nullptr;
    }

    std::vector<StaticVertex> vertices(header.vertex_count);
    std::vector<uint32_t> indices(header.index_count);

    ifs.read(reinterpret_cast<char*>(vertices.data()),
             static_cast<std::streamsize>(header.vertex_count * sizeof(StaticVertex)));
    ifs.read(reinterpret_cast<char*>(indices.data()),
             static_cast<std::streamsize>(header.index_count * sizeof(uint32_t)));

    //SubMeshArray
    std::vector<SubMeshEntry> sub_meshes(header.submesh_count);
    if (header.submesh_count > 0)
    {
        ifs.read(reinterpret_cast<char*>(sub_meshes.data()),
                 static_cast<std::streamsize>(header.submesh_count * sizeof(SubMeshEntry)));
    }

    //MaterialArray
    std::vector<MaterialEntry> materials(header.material_count);
    if (header.material_count > 0)
    {
        ifs.read(reinterpret_cast<char*>(materials.data()),
                 static_cast<std::streamsize>(header.material_count * sizeof(MaterialEntry)));
    }

    std::vector<std::wstring>
        diffuse_paths(header.material_count);
    std::vector<std::wstring>
        normal_paths(header.material_count);
    std::vector<std::wstring>
        specular_paths(header.material_count);
    std::vector<std::wstring>
        height_paths(header.material_count);
    for (uint32 i = 0; i < header.material_count; ++i)
    {
        auto read_path = [&](std::wstring& dst, uint32 len)
        {
            if (len > 0)
            {
                dst.resize(len);
                ifs.read(reinterpret_cast<char*>(dst.data()),
                         static_cast<std::streamsize>(len * sizeof(wchar_t)));
            }
        };
        read_path(diffuse_paths[i],
                  materials[i].diffuse_texture_length);
        read_path(normal_paths[i],
                  materials[i].normal_texture_length);
        read_path(specular_paths[i],
                  materials[i].specular_texture_length);
        read_path(height_paths[i],
                  materials[i].height_texture_length);
    }
    if (!ifs)
    {
        return nullptr;
    }

    VertexData vd = {};
    vd.data = vertices.data();
    vd.total_size = header.vertex_count * sizeof(StaticVertex);
    vd.stride = sizeof(StaticVertex);

    IndexData id = {};
    id.data = indices.data();
    id.total_size = header.index_count * sizeof(uint32_t);
    id.format = DXGI_FORMAT_R32_UINT;

    auto mesh = std::make_unique<Mesh>();
    if (!mesh->Create(device_, vd, id, kStaticVertexLayout))
    {
        return nullptr;
    }
    std::vector<Vec3> col_pos(header.vertex_count);
    for (uint32 i = 0; i < header.vertex_count; ++i)
    {
        col_pos[i] = Vec3(vertices[i].position[0], vertices[i].position[1], vertices[i].position[2]);
    }
    mesh->SetCollisionMesh(std::move(col_pos), std::move(indices));

    //Change MaterialEntry -> MeshMaterialDesc
    std::vector<MeshMaterialDesc> descs(header.material_count);
    for (uint32_t i = 0; i < header.material_count; ++i)
    {
        descs[i].base_color = materials[i].base_color;
        descs[i].diffuse_texture_path = diffuse_paths[i].c_str();
        descs[i].normal_texture_path = normal_paths[i].c_str();
        descs[i].specular_texture_path = specular_paths[i].c_str();
        descs[i].height_texture_path = height_paths[i].c_str();
    }
    mesh->SetMaterialDescs(descs);

    //Change SubMeshEntry -> SubMesh
    std::vector<SubMesh> sub_decs(header.submesh_count);
    for (uint32_t i = 0; i < header.submesh_count; ++i)
    {
        sub_decs[i].index_start = sub_meshes[i].index_start;
        sub_decs[i].index_count = sub_meshes[i].index_count;
        sub_decs[i].material_slot = sub_meshes[i].material_slot;
    }
    mesh->SetSubMeshes(sub_decs);

    Mesh* result = mesh.get();
    cache_.emplace(path, std::move(mesh));
    return result;
}

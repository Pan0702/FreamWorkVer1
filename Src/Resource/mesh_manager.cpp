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
    
    std::ifstream ifs(path,std::ios::binary);
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
        MessageBoxA(nullptr, "Invalid version", "Error", MB_OK);
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
    
    std::vector<std::wstring> diffuse_paths(header.material_count);
    for (uint32_t i = 0; i < header.material_count; ++i)
    {
        const uint32_t len = materials[i].diffuse_texture_length;
        if (len > 0)
        {
            diffuse_paths[i].resize(len);
            ifs.read(reinterpret_cast<char*>(diffuse_paths[i].data()),
                static_cast<std::streamsize>(len * sizeof(wchar_t)));
        }
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
    if (!mesh->Create(device_, vd, id,kStaticVertexLayout))
    {
        return nullptr;
    }
    
    //Change MaterialEntry -> MeshMaterialDesc
    std::vector<MeshMaterialDesc> descs(header.material_count);
    for (uint32_t i = 0; i < header.material_count; ++i)
    {
        descs[i].base_color = materials[i].base_color;
        descs[i].diffuse_texture_path = diffuse_paths[i].c_str();
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

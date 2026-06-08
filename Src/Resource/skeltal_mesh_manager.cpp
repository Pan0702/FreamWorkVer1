#include "skeltal_mesh_manager.h"
#include <fstream>

#include "skeleton.h"
#include "mesh.h"
#include "skmesh_file.h"
#include "vertex_types.h"

SkeletalMeshManager::SkeletalMeshManager() = default;
SkeletalMeshManager::~SkeletalMeshManager() = default;

void SkeletalMeshManager::Initialize(ID3D12Device* device)
{
    device_ = device;
}

void SkeletalMeshManager::Shutdown()
{
    cache_.clear();
    device_ = nullptr;
}

SkeletalMesh* SkeletalMeshManager::Load(const std::string& path)
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
        return nullptr;
    }

    SkMeshFileHeader header = {};
    ifs.read(reinterpret_cast<char*>(&header), sizeof(SkMeshFileHeader));
    if (!ifs)
    {
        MessageBox(nullptr, L"Failed to read SkeltalMesh file", L"Error", MB_OK);
        return nullptr;
    }
    if (std::memcmp(header.magic, kSkMeshMagic, 4) != 0)
    {
        MessageBoxA(nullptr, "Invalid magic", "Error", MB_OK);
        return nullptr;
    }

    if (header.version != kSkMeshVersion)
    {
        MessageBoxA(nullptr, "Invalid version skmesh", "Error", MB_OK);
        return nullptr; 
    }

    if (header.vertex_stride != sizeof(SkinnedVertex))
    {
        MessageBoxA(nullptr, "Invalid vertex skmesh stride", "Error", MB_OK);
        return nullptr;
    }

    if (header.index_stride != sizeof(uint32_t))
    {
        MessageBoxA(nullptr, "Invalid index skmesh stride", "Error", MB_OK);
        return nullptr;
    }

    std::vector<SkinnedVertex> vertices(header.vertex_count);
    std::vector<uint32_t> indices(header.index_count);

    ifs.read(reinterpret_cast<char*>(vertices.data()),
             static_cast<std::streamsize>(header.vertex_count * sizeof(SkinnedVertex)));
    ifs.read(reinterpret_cast<char*>(indices.data()),
             static_cast<std::streamsize>(header.index_count * sizeof(uint32_t)));

    std::vector<SubMeshEntry> sub_meshes(header.submesh_count);
    if (header.submesh_count > 0)
    {
        ifs.read(reinterpret_cast<char*>(sub_meshes.data()),
                 static_cast<std::streamsize>(header.submesh_count * sizeof(SubMeshEntry)));
    }

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
    std::vector<NodeEntry> nodes(header.node_count);
    ifs.read(reinterpret_cast<char*>(nodes.data()),
             static_cast<std::streamsize>(header.node_count * sizeof(NodeEntry)));

    std::vector<std::string> skin(header.node_count);
    for (uint32_t i = 0; i < header.node_count; ++i)
    {
        const uint32_t len = nodes[i].name_length;
        if (len > 0)
        {
            skin[i].resize(len);
            ifs.read(skin[i].data(), len);
        }
    }
    std::vector<SkeletonNode> skel_nodes(header.node_count);
    for (uint32_t i = 0; i < header.node_count; ++i)
    {
        skel_nodes[i].name = skin[i];
        skel_nodes[i].parent_index = nodes[i].parent_index;
        skel_nodes[i].skin_index = nodes[i].skin_index;
        skel_nodes[i].local_bind_transform = nodes[i].local_bind_transform;
        skel_nodes[i].inverse_bind_pose = nodes[i].inverse_bind_pose;
    }
    if (!ifs)
    {
        return nullptr;
    }

    VertexData vd = {};
    vd.data = vertices.data();
    vd.total_size = header.vertex_count * sizeof(SkinnedVertex);
    vd.stride = sizeof(SkinnedVertex);

    IndexData id = {};
    id.data = indices.data();
    id.total_size = header.index_count * sizeof(uint32_t);
    id.format = DXGI_FORMAT_R32_UINT;

    auto sk_mesh = std::make_unique<SkeletalMesh>();
    if (!sk_mesh->Create(device_, vd, id, kSkinnedVertexLayout))
    {
        return nullptr;
    }
    auto sk = std::make_unique<Skeleton>();
    sk->SetNodes(std::move(skel_nodes), header.skin_count);
    sk_mesh->SetSkeleton(std::move(sk));
    //Change MaterialEntry -> MeshMaterialDesc
    std::vector<MeshMaterialDesc> descs(header.material_count);
    for (uint32_t i = 0; i < header.material_count; ++i)
    {
        descs[i].base_color = materials[i].base_color;
        descs[i].diffuse_texture_path = diffuse_paths[i].c_str();
    }
    sk_mesh->SetMaterialDescs(descs);

    //Change SubMeshEntry -> SubMesh
    std::vector<SubMesh> sub_decs(header.submesh_count);
    for (uint32_t i = 0; i < header.submesh_count; ++i)
    {
        sub_decs[i].index_start = sub_meshes[i].index_start;
        sub_decs[i].index_count = sub_meshes[i].index_count;
        sub_decs[i].material_slot = sub_meshes[i].material_slot;
    }
    sk_mesh->SetSubMeshes(sub_decs);

    SkeletalMesh* result = sk_mesh.get();
    cache_[path] = std::move(sk_mesh);
    return result;
}

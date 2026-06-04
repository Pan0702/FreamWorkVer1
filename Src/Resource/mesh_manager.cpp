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
        MessageBox(nullptr, L"Failed to open file", L"Error", MB_OK);
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
    
    mesh->GetMaterialDesc().base_color
    Mesh* result = mesh.get();
    cache_.emplace(path, std::move(mesh));
    return result;   
}

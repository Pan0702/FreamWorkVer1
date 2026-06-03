#pragma once
#include <unordered_map>

#include "mesh.h"
#include "../Graphics/graphics_device.h"

class MeshManager
{
public:
    void Initialize(ID3D12Device* device);
    void Shutdown();
    
    Mesh* Load(const std::string& path);
    
    static MeshManager& Get()
    {
        static MeshManager instance;
        return instance;
    }
    
private:
    ID3D12Device* device_ = nullptr;
    std::unordered_map<std::string,std::unique_ptr<Mesh>> cache_;
    
};

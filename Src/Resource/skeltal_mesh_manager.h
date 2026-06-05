#pragma once
#pragma once
#include <unordered_map>

#include "skeltal_mesh.h"
#include "../Graphics/graphics_device.h"
class SkeltalMeshManager
{
public:
    void Initialize(ID3D12Device* device);
    void Shutdown();
    SkeltalMesh* Load(const std::string& path);
    
    static SkeltalMeshManager& Get()
    {
        static SkeltalMeshManager instance;
        return instance;
    }
    
private:
    ID3D12Device* device_ = nullptr;
    std::unordered_map<std::string,std::unique_ptr<SkeltalMesh>> cache_;
};


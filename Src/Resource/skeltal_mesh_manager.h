#pragma once
#pragma once
#include <unordered_map>

#include "skeltal_mesh.h"
#include "../Graphics/graphics_device.h"
class SkeletalMeshManager
{
public:
    SkeletalMeshManager();
    ~SkeletalMeshManager();
    void Initialize(ID3D12Device* device);
    void Shutdown();
    SkeletalMesh* Load(const std::string& path);
    
    static SkeletalMeshManager& Get()
    {
        static SkeletalMeshManager instance;
        return instance;
    }
    
private:
    ID3D12Device* device_ = nullptr;
    std::unordered_map<std::string,std::unique_ptr<SkeletalMesh>> cache_;
};


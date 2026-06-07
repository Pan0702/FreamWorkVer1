#pragma once
#include <string>
#include <vector>
#include <span>
#include <unordered_map>
#include "../Core/Math/my_math.h"
struct SkeletonNode
{
    std::string name;
    int parent_index = -1;
    Mat local_bind_transform;
    
    int skin_index = -1;
    Mat inverse_bind_pose;
};

class Skeleton
{
public:
    void SetNodes(std::vector<SkeletonNode> nodes, uint32 skin_count);
    uint32 GetSkinCount() const ;
    int32 GetNodeIndex(const std::string& name) const;
    const std::vector<SkeletonNode>& GetNodes() const;
private:
    std::vector<SkeletonNode> nodes_;
    std::unordered_map<std::string, int32> name_to_index_; 
    uint32 skin_count_ = 0;
};



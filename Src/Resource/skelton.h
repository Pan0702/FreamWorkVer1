#pragma once
#include <string>
#include <vector>

#include "../Core/Math/my_math.h"
struct SleltonNode
{
    std::string name;
    int parent_index;
    Mat local_bind_transform;
    
    int skin_index = -1;
    Mat inverse_bind_pose;
};

class Skelton
{
    std::vector<SleltonNode> nodes_;
    uint32_t skin_count_ = 0;
};



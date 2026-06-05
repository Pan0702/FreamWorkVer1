#pragma once
#include <string>
#include <vector>
#include "../Core/Math/my_math.h"
template<class T>
struct Key
{
    float time;
    T value;
};
struct NodeAnimation
{
    std::string name;
    std::vector<Key<Vec3>> position_keys;
    std::vector<Key<Quat>> rotation_keys;
    std::vector<Key<Vec3>> scale_keys;   
};
class AnimationClip
{
private:
    float duration_ = 0.0f;
    float ticks_per_second_ = 0.0f;
    std::vector<NodeAnimation> node_animations_;
};

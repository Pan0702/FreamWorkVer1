#pragma once
#include <string>
#include <vector>
#include "../Core/Math/my_math.h"
struct SkAnimFileHeader;

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
class Animation
{
public:
    void SetData(float duration, float ticks_per_second,
                 std::vector<NodeAnimation> node);
    void SetLooping(bool loop);
    float GetDuration() const;
    float GetTicksPerSecond() const;
    bool IsLooping() const;
    const std::vector<NodeAnimation>& GetNodeAnimations() const;
private:
    float duration_ = 0.0f;
    float ticks_per_second_ = 0.0f;
    std::vector<NodeAnimation> node_animations_;
    bool loop_ = true;
};

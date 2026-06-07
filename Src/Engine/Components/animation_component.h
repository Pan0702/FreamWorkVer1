#pragma once
#include <vector>

#include "../../Core/Math/my_math.h"
#include "../component.h"

class Skeleton;
class Animation;

class AnimationComponent : public Component
{
public:
    void OnAttach(const AttachContext& context) override;
    void Tick(float dt) override;
    void Play(Animation* clip);
    const std::vector<Mat>& GetBonePalette();
    
private:
    Skeleton* skeleton_ = nullptr;
    Animation* clip_ = nullptr;\
    float time_ = 0.0f;
    std::vector<Mat> global_poses_;;
    std::vector<Mat> bone_matrices_;
};

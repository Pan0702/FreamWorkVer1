#pragma once
#include <string>
#include <vector>

#include "../../Core/Math/my_math.h"
#include "../component.h"

struct SkeletonNode;
struct NodeAnimation;
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
    NodeAnimation* FindChannel(Animation* anim,const std::string& name);
    Mat ComposeLocal(NodeAnimation anim, float time,SkeletonNode);
    Skeleton* skeleton_ = nullptr;
    Animation* clip_ = nullptr;\
    float time_ = 0.0f;
    std::vector<Mat> global_poses_;;
    std::vector<Mat> bone_matrices_;
};

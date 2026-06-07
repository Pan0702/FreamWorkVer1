#pragma once
#include <string>
#include <unordered_map>
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
    void AddAnimation(const std::string& name, Animation* clip);
    void Play(const std::string& name, bool loop);
    void Stop();
    float GetCurrentFrame() const;
    void SetPlaySpeed(float speed);
    const std::vector<Mat>& GetBonePalette() const;
    bool IsPlaying() const;
    
private:
    Mat ComposeLocal(const NodeAnimation& anim, float time, const SkeletonNode& node);
    void RebuildChannels();
    
    Skeleton* skeleton_ = nullptr;
    Animation* clip_ = nullptr;
    float time_ = 0.0f;
    std::vector<Mat> global_poses_;
    std::vector<Mat> bone_matrices_;
    std::vector<const NodeAnimation*> node_channels_;
    bool channels_dirty_ = true;
    bool loop_ = true;
    bool playing_ = false;
    float play_speed_ = 1.0f;
    std::unordered_map<std::string, Animation*> animation_deta_;
};


#include "animation_component.h"

#include <map>

#include "skeletal_mesh.h"
#include "../actor.h"
#include "../../Resource/animation.h"
#include "../../Resource/skeltal_mesh.h"
#include "../../Resource/skeleton.h"

namespace
{
    Vec3 SampleVec3(const std::vector<Key<Vec3>>& vec, float t, const Vec3 fb)
    {
        if (vec.empty())
        {
            return fb;
        }
        if (vec.size() == 1 || t <= vec.front().time)
        {
            return vec.front().value;
        }
        if (t >= vec.back().time)
        {
            return vec.back().value;
        }
        for (size_t i = 0; i + 1 < vec.size(); ++i)
        {
            if (t < vec[i + 1].time)
            {
                float a = (t - vec[i].time) / (vec[i + 1].time - vec[i].time);
                return Lerp(vec[i].value, vec[i + 1].value, a);
            }
        }
        return vec.back().value;
    }

    Quat SampleQuat(const std::vector<Key<Quat>>& vec, float t, const Quat fb)
    {
        if (vec.empty())
        {
            return fb;
        }
        if (vec.size() == 1 || t <= vec.front().time)
        {
            return vec.front().value;
        }
        if (t >= vec.back().time)
        {
            return vec.back().value;
        }
        for (size_t i = 0; i + 1 < vec.size(); ++i)
        {
            if (t < vec[i + 1].time)
            {
                float a = (t - vec[i].time) / (vec[i + 1].time - vec[i].time);
                return Slerp(vec[i].value, vec[i + 1].value, a);
            }
        }
        return vec.back().value;
    }
}

void AnimationComponent::OnAttach(const AttachContext& context)
{
    Component::OnAttach(context);
}

void AnimationComponent::Tick(float dt)
{
    if (!skeleton_)
    {
        if (Actor* owner = GetOwner())
            if (auto* sk_mesh_comp = owner->GetComponent<SkeletalMeshComponent>())
                if (auto* sk_mesh = sk_mesh_comp->GetSkeltalMesh())
                {
                    skeleton_ = sk_mesh->GetSkeleton();
                    channels_dirty_ = true;
                }
    }

    if (!skeleton_)
    {
        return;
    }
    const auto& nodes = skeleton_->GetNodes();
    global_poses_.resize(nodes.size());

    if (playing_ && clip_ && clip_->GetTicksPerSecond() > 0.0f)
    {
        time_ += dt * clip_->GetTicksPerSecond() * play_speed_;
        const float dur = clip_->GetDuration();
        if (dur > 0.0f)
        {
            if (loop_)
            {
                time_ = std::fmod(time_, clip_->GetDuration());
            }else if (time_ >= dur)
            {
                time_ = dur;
                playing_ = false;
            }
        }
    }
    
    if (channels_dirty_)
    {
        RebuildChannels();
    }
    
    for (size_t i = 0; i < nodes.size(); i++)
    {
        Mat local = nodes[i].local_bind_transform;
        if (i < node_channels_.size() && node_channels_[i])
        {
            local = ComposeLocal(*node_channels_[i], time_, nodes[i]);
        }

        if (nodes[i].parent_index < 0)
        {
            global_poses_[i] = local;
        }
        else
        {
            const int parent_index = nodes[i].parent_index;
            if (static_cast<size_t>(parent_index) >= i || static_cast<size_t>(parent_index) >= global_poses_.size())
            {
                global_poses_[i] = local;
            }
            else
            {
                global_poses_[i] = local * global_poses_[static_cast<size_t>(parent_index)];
            }
        }
    }
    bone_matrices_.assign(skeleton_->GetSkinCount(), Identity());
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const int skin_index = nodes[i].skin_index;
        if (skin_index >= 0 && static_cast<size_t>(skin_index) < bone_matrices_.size())
        {
            bone_matrices_[static_cast<size_t>(skin_index)] =
                nodes[i].inverse_bind_pose * global_poses_[i];
        }
    }

    Component::Tick(dt);
}

void AnimationComponent::AddAnimation(const std::string& name, Animation* clip)
{
    if (animation_deta_.contains(name))
    {
        return;
    }
    animation_deta_[name] = clip;
}

void AnimationComponent::Play(const std::string& name,bool loop)
{
    clip_ = animation_deta_.at(name);
    time_ = 0.0f;
    channels_dirty_ = true;
    playing_ = true;
    loop_ = loop;
}


void AnimationComponent::Stop()
{
    clip_ = nullptr;
    playing_ = false;
    time_ = 0.0f;
}

float AnimationComponent::GetCurrentFrame() const
{
    return time_;
}

void AnimationComponent::SetPlaySpeed(float speed)
{
    play_speed_ = speed;
}



void AnimationComponent::RebuildChannels()
{
    node_channels_.clear();
    channels_dirty_ = false;
    if (!skeleton_ || !clip_)
    {
        return;
    }
    std::unordered_map<std::string, const NodeAnimation*> channels;
    for (const NodeAnimation& node : clip_->GetNodeAnimations())
    {
        channels[node.name] = &node;
    }
    
    const auto& nodes = skeleton_->GetNodes();
    node_channels_.resize(nodes.size(),nullptr);
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        auto it = channels.find(nodes[i].name);
        node_channels_[i] = it != channels.end() ? it->second : nullptr;
    }
}

Mat AnimationComponent::ComposeLocal(const NodeAnimation& anim, float time, const SkeletonNode& node)
{
    Vec3 bt(0,0,0);
    Vec3 bs(1,1,1);
    Quat brot(QuatIdentity());
    
    Decompose(node.inverse_bind_pose, bs, brot, bt);
    
    Vec3 p = SampleVec3(anim.position_keys, time, bt);
    Quat r = SampleQuat(anim.rotation_keys, time, brot);
    Vec3 s = SampleVec3(anim.scale_keys, time, bs);

    return Scale(s) * ToMat(r) * Translate(p);
}

const std::vector<Mat>& AnimationComponent::GetBonePalette() const
{
    return bone_matrices_;
}

bool AnimationComponent::IsPlaying() const
{
    return playing_;
}
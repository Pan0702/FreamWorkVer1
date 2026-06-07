#include "animation_component.h"

#include <map>

#include "skeletal_mesh.h"
#include "../actor.h"
#include "../../Resource/animation.h"
#include "../../Resource/skeltal_mesh.h"
#include "../../Resource/skeleton.h"

namespace
{
    struct LocalPose
    {
        Vec3 t;
        Quat r;
        Vec3 s;
    };

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

    LocalPose SampleNode(const NodeAnimation* ch, float time, const SkeletonNode& node)
    {
        Vec3 bt(0, 0, 0), bs(1, 1, 1);
        Quat br = QuatIdentity();
        Decompose(node.local_bind_transform, bs, br, bt);

        if (!ch) return {.t = bt, .r = br, .s = bs}; // チャンネル無し＝丸ごと bind

        LocalPose lp;
        lp.t = SampleVec3(ch->position_keys, time, bt);
        lp.r = SampleQuat(ch->rotation_keys, time, br);
        lp.s = SampleVec3(ch->scale_keys, time, bs);
        return lp;
    }

    LocalPose BlendPose(const LocalPose& a, const LocalPose& b, float t)
    {
        LocalPose lp;
        lp.t = Lerp(a.t, b.t, t);
        lp.r = Slerp(a.r, b.r, t);
        lp.s = Lerp(a.s, b.s, t);
        return lp;
    }

    Mat ToLocalMat(const LocalPose& lp)
    {
        return Scale(lp.s) * ToMat(lp.r) * Translate(lp.t);
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
    
    auto advance = [&](const Animation* c, float& t)
    {
        if (c && c->GetTicksPerSecond() > 0.0f)
        {
            t += dt * c->GetTicksPerSecond() * play_speed_;
            float dur = c->GetDuration();
            if (dur > 0.0f) t = std::fmod(t, dur);
        }
    };

    if (playing_)
    {
        advance(clip_, time_);
        if (fading_) advance(prev_clip_, prev_time_);
    }

    // フェード進行（weight: 0=前, 1=現在）
    float w = 1.0f;
    if (fading_ && fade_duration_ > 0.0f)
    {
        fade_elapsed_ += dt;
        w = fade_elapsed_ / fade_duration_;
        if (w >= 1.0f)
        {
            // フェード完了 
            w = 1.0f;
            fading_ = false;
            prev_clip_ = nullptr;
            prev_channels_.clear();
        }
    }

    if (channels_dirty_)
    {
        RebuildChannels();
    }

    for (size_t i = 0; i < nodes.size(); i++)
    {
        const NodeAnimation* cur_ch = (i < node_channels_.size()) ? node_channels_[i] : nullptr;
        LocalPose pose = SampleNode(cur_ch, time_, nodes[i]);

        if (fading_)
        {
            // 前アニメも評価してブレンド
            const NodeAnimation* prev_ch = (i < prev_channels_.size()) ? prev_channels_[i] : nullptr;
            LocalPose prev_pose = SampleNode(prev_ch, prev_time_, nodes[i]);
            pose = BlendPose(prev_pose, pose, w); 
        }
        Mat local = ToLocalMat(pose);
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

void AnimationComponent::AddAnimation(const std::string& name, Animation* clip,bool loop)
{
    clip->SetLooping(loop);
    if (animation_deta_.contains(name))
    {
        return;
    }
    animation_deta_[name] = clip;
}

void AnimationComponent::Play(const std::string& name)
{
    clip_ = animation_deta_.at(name);
    time_ = 0.0f;
    channels_dirty_ = true;
    playing_ = true;
    loop_ = clip_->IsLooping();
}

void AnimationComponent::CorossFade(const std::string& name, float fade_time)
{
    if (fade_time <= 0.0f || !clip_)
    {
        Play(name);
        return;
    }
    prev_clip_ = clip_;
    prev_time_ = time_;
    prev_channels_ = std::move(node_channels_);

    clip_ = animation_deta_.at(name);
    time_ = 0.0f;
    channels_dirty_ = true;

    fade_duration_ = fade_time;
    fade_elapsed_ = 0.0f;
    playing_ = true;
    fading_ = true;
    loop_ = clip_->IsLooping();
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
    node_channels_.resize(nodes.size(), nullptr);
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        auto it = channels.find(nodes[i].name);
        node_channels_[i] = it != channels.end() ? it->second : nullptr;
    }
}

Mat AnimationComponent::ComposeLocal(const NodeAnimation& anim, float time, const SkeletonNode& node)
{
    Vec3 bt(0, 0, 0);
    Vec3 bs(1, 1, 1);
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

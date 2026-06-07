#include "animation_component.h"

#include "skeletal_mesh.h"
#include "../actor.h"
#include "../../Resource/animation.h"
#include "../../Resource/skeltal_mesh.h"
#include "../../Resource/skeleton.h"

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
                    skeleton_ = sk_mesh->GetSkeleton();
    }
    
    if (!skeleton_)return;
    const auto& nodes = skeleton_->GetNodes();
    global_poses_.resize(nodes.size());
    
    if (clip_ && clip_->GetTicksPerSecond() > 0.0f)
    {
        time_ += dt * clip_->GetTicksPerSecond();
        if (clip_->GetDuration() > 0.0f)
        {
            time_ = std::fmod(time_, clip_->GetDuration());
        }
    }
    for (size_t i = 0; i < nodes.size(); i++)
    {
        Mat local = nodes[i].local_bind_transform;
        if( NodeAnimation* node = FindChannel(clip_, nodes[i].name))
        {
            local = ComposeLocal(*node,time_,nodes[i]); 
        }
        
        if (nodes[i].parent_index < 0)
        {
            global_poses_[i] = local;
        }
        else
        {
            global_poses_[i] = local * global_poses_[nodes[i].parent_index];
        }
    }
    bone_matrices_.assign(skeleton_->GetSkinCount(), Identity());
    for (size_t i = 0; i < bone_matrices_.size(); ++i)
    {
        if (nodes[i].skin_index >= 0)
        {
            bone_matrices_[nodes[i].skin_index] = 
                nodes[i].inverse_bind_pose * global_poses_[i];
        }
    }
    {
        
    }
    Component::Tick(dt);
}

void AnimationComponent::Play(Animation* clip)
{
}

const std::vector<Mat>& AnimationComponent::GetBonePalette()
{
}

NodeAnimation* AnimationComponent::FindChannel(Animation* anim, const std::string& name)
{
    
}

Mat AnimationComponent::ComposeLocal()
{
    
}

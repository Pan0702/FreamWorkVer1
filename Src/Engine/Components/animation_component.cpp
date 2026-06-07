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

    Component::Tick(dt);
}

void AnimationComponent::Play(Animation* clip)
{
}

const std::vector<Mat>& AnimationComponent::GetBonePalette()
{
}

#include "skeletal_mesh.h"

SkeletalMeshComponent::SkeletalMeshComponent(SkeltalMesh* skeltal_mesh)
{
    skeltal_mesh_ = skeltal_mesh;
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
    OnDetach();
}

void SkeletalMeshComponent::OnAttach(const AttachContext& context)
{
    Component::OnAttach(context);
}

void SkeletalMeshComponent::OnDetach()
{
    Component::OnDetach();
}

Animation* SkeletalMeshComponent::GetAnimation(const std::string& name)
{
    auto it = animations_.find(name);
    if (it != animations_.end())
    {
        return it->second;
    }
    return nullptr;
}

void SkeletalMeshComponent::SetAnimation(const std::string& name, Animation* animation)
{
    animations_.emplace(name,animation);
}

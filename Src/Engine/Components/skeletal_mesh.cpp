#include "skeletal_mesh.h"
#include "../attach_context.h"
#include "../../Renderer/skinned_mesh_renderer.h"
#include "../../Resource/material_slot.h"

SkeletalMeshComponent::SkeletalMeshComponent(SkeletalMesh* skeletal_mesh,MaterialSlot* material_slot)
{
    skeltal_mesh_ = skeletal_mesh;
    material_slot_ = material_slot;
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
    OnDetach();
}

void SkeletalMeshComponent::OnAttach(const AttachContext& context)
{
    renderer_ = context.skinned_mesh_renderer;
    if (renderer_) renderer_->Register(this);
    Component::OnAttach(context);
}

void SkeletalMeshComponent::OnDetach()
{
    if (renderer_) renderer_->Unregister(this);
    renderer_ = nullptr;
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

SkeletalMesh* SkeletalMeshComponent::GetSkeltalMesh() const
{
    return skeltal_mesh_;
}

MaterialSlot* SkeletalMeshComponent::GetMaterialSlot() const
{
    return material_slot_;
}

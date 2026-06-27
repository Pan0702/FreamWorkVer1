#include "skeletal_mesh.h"
#include "../attach_context.h"
#include "../../Renderer/skinned_mesh_renderer.h"
#include "../../Resource/material_slot.h"
#include "../../Resource/skeltal_mesh.h"
#include "../actor.h"
struct AABB;

SkeletalMeshComponent::SkeletalMeshComponent(SkeletalMesh* skeletal_mesh, MaterialSlot* material_slot)
{
    skeltal_mesh_ = skeletal_mesh;
    material_slot_ = material_slot;
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
    OnDetach();
}

void SkeletalMeshComponent::Tick(float dt)
{
    Component::Tick(dt);
    if (is_draw_)
    {
        DebugDraw();
    }
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
    animations_.emplace(name, animation);
}

void SkeletalMeshComponent::DebugDraw() const
{
    const AABB& b = skeltal_mesh_->GetBounds();
    const auto& tf = GetOwner()->GetTransform();
    Vec3 local_center = (b.min + b.max) * 0.5f;
    Vec3 size = (b.max - b.min);
    Vec3 world_center = tf.position + Vec3(local_center.x * tf.scale.x,
                                           local_center.y * tf.scale.y,
                                           local_center.z * tf.scale.z);
    Vec3 world_size = Vec3(size.x * tf.scale.x, size.y * tf.scale.y,
                           size.z * tf.scale.z);
    Debug::Get().DrawBox3D(world_center, world_size, Vec4(0.1f, 0.1f, 0.1f, 0.3f),
                           Vec3(0, 0, 0));
}

SkeletalMesh* SkeletalMeshComponent::GetSkeltalMesh() const
{
    return skeltal_mesh_;
}

MaterialSlot* SkeletalMeshComponent::GetMaterialSlot() const
{
    return material_slot_;
}

void SkeletalMeshComponent::SetDraw(bool is_draw)
{
    is_draw_ = is_draw;
}

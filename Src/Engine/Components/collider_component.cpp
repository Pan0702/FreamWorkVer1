#include <cmath>
#include "collider_component.h"
#include "../actor.h"
#include "transform_component.h"
#include "../collision_world.h"
void ColliderComponent::SetOnBeginOverlap(OverlapCallback callback)
{
    on_begin_ = std::move(callback);
}

void ColliderComponent::SetOnEndOverlap(OverlapCallback callback)
{
    on_end_ = std::move(callback);
}

void ColliderComponent::OnAttach(const AttachContext& context)
{
    collision_world_ = context.collision_world;
    Component::OnAttach(context);
    if (collision_world_)
    {
        collision_world_->Register(this);
    }
}

void ColliderComponent::OnDetach()
{
    Component::OnDetach();
    if (collision_world_)
    {
        collision_world_->Unregister(this);
    }
}

bool ColliderComponent::TryGetColliderTransform(Vec3* center, Vec3* abs_scale) const
{
    Actor* actor = GetOwner();
    if (!actor)
    {
        return false;
    }
    const TransformComponent& transform = actor->GetTransform();
    *center = transform.position;
    *abs_scale = Vec3{
        std::abs(transform.scale.x),
        std::abs(transform.scale.y),
        std::abs(transform.scale.z)
    };
    return true;
}

void ColliderComponent::InvokeBeginOverlap(const ColliderComponent* other) const
{
    if (on_begin_)
    {
        on_begin_(this,other);
    }
}

void ColliderComponent::InvokeEndOverlap(const ColliderComponent* other)
{
    if (on_end_)
    {
        on_end_(this,other);
    }   
}

void ColliderComponent::InvokeHit(ColliderComponent* other, const ContactInfo& info)
{
    if (on_hit_)
    {
        on_hit_(this,other->GetOwner(),other,info);
    }
}

void ColliderComponent::SetColor(const Vec4& color)
{
    color_ = color;
}

void ColliderComponent::SetDraw(bool draw)
{
    is_draw_ = draw;
}

bool ColliderComponent::IsDraw() const
{
    return is_draw_;
}

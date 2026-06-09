#include <cmath>
#include "collider_component.h"
#include "../actor.h"
#include "transform_component.h"
#include "../../Debug/debug.h"
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
    Component::OnAttach(context);
    Actor* actor = GetOwner();
    if (!actor || !actor->GetComponent<TransformComponent>())
    {
        has_transform_ = false;
        DEBUG_LOG("ColliderComponent requires TransformComponent");
    }
    context.collision_world->Register(this);
}

void ColliderComponent::OnDetach()
{
    Component::OnDetach();
    Unregister();
}

bool ColliderComponent::TryGetColliderTransform(Vec3* center, Vec3* abs_scale)
{
    Actor* actor = GetOwner();
    if (!actor)
    {
        return false;
    }
    auto* transform = actor->GetComponent<TransformComponent>();
    if (!transform)
    {
        return false;
    }
    
    *center = transform->position;
    *abs_scale = Vec3{
        std::abs(transform->scale.x),
        std::abs(transform->scale.y),
        std::abs(transform->scale.z)
    };
    return true;
}

bool ColliderComponent::HasTransform() const
{
    return has_transform_;
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

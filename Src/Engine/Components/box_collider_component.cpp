#include "box_collider_component.h"

void BoxColliderComponent::SetHalfSize(const Vec3& size)
{
    half_size = size;
}

const Vec3& BoxColliderComponent::GetHalfSize() const
{
    return half_size;
}

ColliderShape BoxColliderComponent::GetColliderShape() const
{
    return ColliderShape::kBox;
}

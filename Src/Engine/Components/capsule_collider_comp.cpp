#include "capsule_collider_comp.h"
#include "../../Core/Math/intersect.h"
void CapsuleColliderComponent::SetHalfSize(const Vec3& size)
{
}

const Vec3& CapsuleColliderComponent::GetHalfSize() const
{
    
}

ColliderShape CapsuleColliderComponent::GetColliderShape() const
{
    return ColliderShape::kCapsule;
}

Capsule CapsuleColliderComponent::GetColliderCapsuleData() const
{
}

void CapsuleColliderComponent::DrawDebug() const
{
}

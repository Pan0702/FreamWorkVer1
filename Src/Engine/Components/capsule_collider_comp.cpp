#include "capsule_collider_comp.h"

#include <algorithm>
#include "../../Core/Math/intersect.h"
void CapsuleColliderComponent::SetHalfSize(const Vec3& size)
{
    half_size = size;
}

const Vec3& CapsuleColliderComponent::GetHalfSize() const
{
    return half_size;
}

ColliderShape CapsuleColliderComponent::GetColliderShape() const
{
    return ColliderShape::kCapsule;
}

Capsule CapsuleColliderComponent::GetColliderCapsuleData() const
{
    Vec3 center, scale;
    TryGetColliderTransform(&center, &scale); 
    Capsule c;
    c.center = center;
    c.dir = Vec3(0,1,0);
    
    // 半径
    c.radius = half_size.x * scale.x;
    float height = half_size.y * scale.y;
    c.height = height - c.radius;
    
    //半径より低かったら球にする
    c.height = (std::max)(c.height, 0.0f);
    return c;
}

void CapsuleColliderComponent::DrawDebug() const
{
}

#include "box_collider_component.h"

#include "../actor.h"
#include "../../Core/Math/intersect.h"

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

Box BoxColliderComponent::GetColliderBoxData() const
{
    Vec3 center, scale;
    TryGetColliderTransform(&center, &scale);   // scale ÇÕíÜÇ≈ abs çœÇ›
    const Vec3 world_half{
        half_size.x * scale.x,
        half_size.y * scale.y,
        half_size.z * scale.z,
    };
    Box box;
    box.min = center - world_half;
    box.max = center + world_half;
    return box;
}

void BoxColliderComponent::DrawDebug() const
{
    Box box = GetColliderBoxData();
    
    Vec3 center = (box.min + box.max) * kHalfSize;
    Vec3 size = (box.max - box.min) * kHalfSize;
    Debug::Get().DrawBox3D(center, size, color_,Vec3(0,0,0));
}

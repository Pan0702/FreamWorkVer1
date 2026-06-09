#include "sphere_collider_componet.h"
#include "../../Core/Math/intersect.h"
void SphereColliderComponent::SetRadius(float radius)
{
    local_radius_ = radius;
}

float SphereColliderComponent::GetRadius() const
{
    return local_radius_;
}

ColliderShape SphereColliderComponent::GetColliderShape() const
{
    return ColliderShape::kSphere;
}

Sphere SphereColliderComponent::GetColliderShapeData() const
{
    Vec3 center, scale;
    TryGetColliderTransform(&center, &scale);
    const float radius = local_radius_ * (std::max)((std::max)(scale.x,scale.y),scale.z);
    Sphere data;
    data.center = center;
    data.radius = radius;
    return data;
}

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

void SphereColliderComponent::GetColliderShapeData(Sphere& data)
{
    Vec3 center, scale;
    TryGetColliderTransform(&center, &scale);
    const float radius = local_radius_ * (std::max)(scale.x,scale.y,scale.z);
    data.center = center;
    data.radius = radius;
}

#pragma once
#include "collider_component.h"

struct Sphere;

class SphereColliderComponent : public ColliderComponent
{
public:
    void SetRadius(float radius);
    float GetRadius() const;
    ColliderShape GetColliderShape() const override;
    Sphere GetColliderShapeData() const;
private:
    float local_radius_ = 0.0f;
};

#pragma once
#include "collider_component.h"
struct Box;
class BoxColliderComponent : public ColliderComponent
{
public:
    void SetHalfSize(const Vec3& size);
    const Vec3& GetHalfSize() const;
    ColliderShape GetColliderShape() const override;
    Box GetColliderBoxData() const;
private:
    Vec3 half_size = {0,0,0};
};

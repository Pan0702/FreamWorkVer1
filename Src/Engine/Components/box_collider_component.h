#pragma once
#include "collider_component.h"

class BoxColliderComponent : public ColliderComponent
{
public:
    void SetHalfSize(const Vec3& size);
    const Vec3& GetHalfSize() const;
    ColliderShape GetColliderShape() const override;
    
private:
    Vec3 half_size = {0,0,0};
};

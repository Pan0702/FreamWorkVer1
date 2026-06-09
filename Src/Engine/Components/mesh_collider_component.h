#pragma once
#include "collider_component.h"

class MeshColliderComponent : public ColliderComponent
{
public:
    ColliderShape GetColliderShape() const override;
    void OnAttach(const AttachContext& context) override;
};

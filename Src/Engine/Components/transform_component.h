#pragma once
#include "../../Core/Math/my_math.h"
#include "../component.h"

class TransformComponent : public Component
{
public:
    Mat Matrix() const;

    Vec3 position = {0.0f, 0.0f, 0.0f};
    Vec3 rotation = {0.0f, 0.0f, 0.0f};
    Vec3 scale = {1.0f, 1.0f, 1.0f};
};

#include "transform_component.h"

Mat TransformComponent::Matrix() const
{
    return Scale(scale) * RotateX(rotation.x) * RotateY(rotation.y) * RotateZ(rotation.z) * Translate(position);
}

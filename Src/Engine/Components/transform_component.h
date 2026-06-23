#pragma once
#include "../../Core/Math/my_math.h"
#include "../component.h"

/**
 * @brief TransformComponentのデータと処理をまとめる型。
 */
class TransformComponent : public Component
{
public:
    /**
     * @brief Transform の位置・回転・スケールから行列を作成する。
     * @return Transform の位置・回転・スケールを合成したワールド行列。
     */
    Mat Matrix() const;

    Vec3 position = {0.0f, 0.0f, 0.0f};
    Vec3 rotation = {0.0f, 0.0f, 0.0f};
    Vec3 scale = {1.0f, 1.0f, 1.0f};
    Vec3 forward = {0.0f, 0.0f, 1.0f};
    Vec3 up = {0.0f, 1.0f, 0.0f};
    Vec3 right = {1.0f, 0.0f, 0.0f};
};

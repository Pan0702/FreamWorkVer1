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
     * @brief Matrixを行う関数。
     * @return 戻り値。
     */
    Mat Matrix() const;

    Vec3 position = {0.0f, 0.0f, 0.0f};
    Vec3 rotation = {0.0f, 0.0f, 0.0f};
    Vec3 scale = {1.0f, 1.0f, 1.0f};
};

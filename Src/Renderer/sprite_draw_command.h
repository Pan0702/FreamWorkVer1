#pragma once
#include "../Core/Math/my_math.h"

class Texture2D;
/**
 * @brief SpriteDrawCommandのデータと処理をまとめる型。
 */
struct SpriteDrawCommand
{
    Texture2D* texture = nullptr;
    Vec2 position = {0.0f, 0.0f};
    Vec3 world_position = {0.0f, 0.0f, 0.0f};
    Mat world = Identity();
    Vec2 size = {0.0f, 0.0f};
    Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec4 src_rect = {0.0f, 0.0f, 1.0f, 1.0f};
    float rotation = 0.0f;
    bool use_texture = true;
    uint64_t sort_key = 0;
};

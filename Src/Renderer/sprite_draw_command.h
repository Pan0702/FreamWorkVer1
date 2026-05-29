#pragma once
#include "../Core/Math/my_math.h"

class Texture2D;

struct SpriteDrawCommand
{
    Texture2D* texture = nullptr;
    Vec2 position = {0.0f, 0.0f};
    Vec2 size = {0.0f, 0.0f};
    Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec4 src_rect = {0.0f, 0.0f, 1.0f, 1.0f};
    float rotation = 0.0f;
    bool use_texture = true;
    uint64_t sort_key = 0;
};

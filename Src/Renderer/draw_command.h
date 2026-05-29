#pragma once
#include "../Core/Math/my_math.h"

class Material;
class Mesh;

struct DrawCommand
{
    Mesh* mesh = nullptr;
    Material* material = nullptr;
    Mat world = Identity();
    float depth = 0.0f;
    uint64_t sort_key = 0;
};

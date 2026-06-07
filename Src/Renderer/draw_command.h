#pragma once
#include <vector>

#include "../Core/Math/my_math.h"

class MaterialSlot;
class Material;
class Mesh;
class SkeletalMesh;
struct DrawCommand
{
    Mesh* mesh = nullptr;
    MaterialSlot* material_slot = nullptr;
    Mat world = Identity();
    float depth = 0.0f;
    uint64_t sort_key = 0;
};

struct SkinnedDrawCommand
{
    SkeletalMesh* mesh = nullptr;
    MaterialSlot* material_slot = nullptr;
    Mat world = Identity();
    const std::vector<Mat>* bone_palette = nullptr;
    float depth = 0.0f;
    uint64_t sort_key = 0;   
};

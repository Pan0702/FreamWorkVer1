#pragma once
#include <vector>

#include "../Core/Math/my_math.h"

class MaterialSlot;
class Material;
class Mesh;
class SkeletalMesh;
class Texture2D;
/**
 * @brief DrawCommandのデータと処理をまとめる型。
 */
struct MeshDrawCommand
{
    Mesh* mesh = nullptr;
    MaterialSlot* material_slot = nullptr;
    Mat world = Identity();
    float depth = 0.0f;
    uint64_t sort_key = 0;
};

/**
 * @brief SkinnedDrawCommandのデータと処理をまとめる型。
 */
struct SkinnedDrawCommand
{
    SkeletalMesh* mesh = nullptr;
    MaterialSlot* material_slot = nullptr;
    Mat world = Identity();
    std::vector<Mat> bone_palette;   
    float depth = 0.0f;
    uint64_t sort_key = 0;
};

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
struct InstancedMeshDrawCommand
{
    Mesh* mesh = nullptr;
    MaterialSlot* material_slot = nullptr;
    uint32 instance_offset = 0;
    uint32 instance_count = 0;
    uint64 sort_key = 0;
};
struct GPUInstanceData
{
    Mat world;
};


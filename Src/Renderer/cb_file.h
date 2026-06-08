#pragma once

//ボーンの最大数
constexpr int kMaxBones = 128;
namespace CB
{
/**
 * @brief MeshObjectCBのデータと処理をまとめる型。
 */
    struct MeshObjectCB
    {
        Mat wvp;
        Mat world;
    };
/**
 * @brief LightCBのデータと処理をまとめる型。
 */
    struct LightCB
    {
        Vec4 light_dir;
        Vec4 light_color;
        Vec4 ambient;
        Vec4 camera_pos;
    };
/**
 * @brief MaterialCBのデータと処理をまとめる型。
 */
    struct MaterialCB
    {
        Vec4 base_color;
        int has_texture;
        float specular_intensity;
        float specular_power;
        int has_normal_map;;
    };
/**
 * @brief BoneCBのデータと処理をまとめる型。
 */
    struct BoneCB
    {
        Mat bones[kMaxBones];
    };
}


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
        Vec4 light_pos;
        Vec4 light_color;
        Vec4 sky_color;
        Vec4 ground_color;
        Vec4 camera_pos;
        Mat light_view_proj;
    };
/**
 * @brief MaterialCBのデータと処理をまとめる型。
 */
    struct MaterialCB
    {
        Vec4 base_color; //rgb + a //
        int has_texture;
        int has_normal_map;
        float metallic; // 1 = 金属 //
        float roughness; // 0 = つるつる　// 
    };
/**
 * @brief BoneCBのデータと処理をまとめる型。
 */
    struct BoneCB
    {
        Mat bones[kMaxBones];
    };
    
    struct ShadowObjectCB
    {
        Mat wvp;
    };
}

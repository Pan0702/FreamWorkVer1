#pragma once

//ボーンの最大数
constexpr int kMaxBones = 128;

enum MaterialFlags : uint8
{
    kMatHasTexture = 1 << 0,
    kMatHasNormalMap = 1 << 1,
    kMatHasSpecular = 1 << 2,
    kMatHasHeight = 1 << 3,
};


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
        Vec4 camera_pos;
        Mat light_view_proj;
    };

    /**
     * @brief MaterialCBのデータと処理をまとめる型。
     */
    struct MaterialCB
    {
        Vec4 base_color; //rgb + a //
        uint32 flag; // 画像を保持してるかのFlag //
        float metallic; // 1 = 金属 //
        float roughness; // 0 = つるつる　// 
        float height_scale; // 1 = 凸凹　// 
    };

    /**
     * @brief BoneCBのデータと処理をまとめる型。
     */
    struct BoneCB
    {
        Mat bones[kMaxBones];
    };

    /**
     * @brief シャドウパスで使うオブジェクト単位の定数バッファ。
     */
    struct ShadowObjectCB
    {
        Mat wvp;
    };
}

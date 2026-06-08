#pragma once

//É{Å[ÉìÇÃç≈ëÂêî
constexpr int kMaxBones = 128;
namespace CB
{
    //CB = constant buffer
    struct MeshObjectCB
    {
        Mat wvp;
        Mat world;
    };
    struct LightCB
    {
        Vec4 light_dir;
        Vec4 light_color;
        Vec4 ambient;
        Vec4 camera_pos;
    };
    
    struct MaterialCB
    {
        Vec4 base_color;
        int has_texture;
        float specular_intensity;
        float specular_power;
        int has_normal_map;;
    };
    struct BoneCB
    {
        Mat bones[kMaxBones];
    };
}


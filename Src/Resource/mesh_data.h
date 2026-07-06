#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
/**
 * @brief VertexDataのデータと処理をまとめる型。
 */
struct VertexData
{
    const void* data;
    uint32 total_size;
    uint32 stride;   
};
/**
 * @brief IndexDataのデータと処理をまとめる型。
 */
struct IndexData
{
    const void* data;
    uint32 total_size;
    DXGI_FORMAT format;
};
/**
 * @brief MeshMaterialDescのデータと処理をまとめる型。
 */
struct MeshMaterialDesc
{
    Vec4 base_color;
    float metallic = 0.0f;
    float roughness = 0.8f;
    std::wstring diffuse_texture_path;
    std::wstring normal_texture_path;
    std::wstring specular_texture_path;  
    std::wstring height_texture_path; 
};
/**
 * @brief SubMeshのデータと処理をまとめる型。
 */
struct SubMesh
{
    uint32 index_start;
    uint32 index_count;
    uint32 material_slot;   
};

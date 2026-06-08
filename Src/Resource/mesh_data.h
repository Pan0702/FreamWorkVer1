#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
struct VertexData
{
    const void* data;
    uint32 total_size;
    uint32 stride;   
};
struct IndexData
{
    const void* data;
    uint32 total_size;
    DXGI_FORMAT format;
};

struct MeshMaterialDesc
{
    Vec4 base_color;
    std::wstring diffuse_texture_path;
};

struct SubMesh
{
    uint32 index_start;
    uint32 index_count;
    uint32 material_slot;   
};
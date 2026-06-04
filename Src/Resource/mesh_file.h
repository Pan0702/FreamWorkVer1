#pragma once
#include <cstdint>
#include "../Core/Math/vec4.h" 
struct MeshFileHeader
{
    char magic[4];
    uint32_t version;
    uint32_t vertex_count;
    uint32_t index_count;   
    uint32_t vertex_stride;
    uint32_t index_stride;
    uint32_t submesh_count;
    uint32_t material_count;
    uint32_t reserved[2];//予備領域
};

struct SubMesh
{
    uint32_t index_start;
    uint32_t index_count;
    uint32_t material_slot;
};

struct Material
{
    Vec4 base_color;
    uint32_t diffuse_texture_length;
    uint32_t reserved;
};
static constexpr char kMeshMagic[4] = {'M', 'E', 'S', 'H'};
static constexpr uint32_t kMeshVersion = 3;

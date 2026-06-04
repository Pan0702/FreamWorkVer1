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
    Vec4 MaterialColor;
    uint32_t diffuse_path_length;;
    uint32_t reserved[2];//予備領域
};

static constexpr char kMeshMagic[4] = {'M', 'E', 'S', 'H'};
static constexpr uint32_t kMeshVersion = 2;

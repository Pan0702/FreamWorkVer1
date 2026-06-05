#pragma once
#include <cstdint>
#include "mesh_format_common.h"  // SubMeshEntry / MaterialEntry（.skmesh と共有）

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

static constexpr char kMeshMagic[4] = {'M', 'E', 'S', 'H'};
static constexpr uint32_t kMeshVersion = 3;

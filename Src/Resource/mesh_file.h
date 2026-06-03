#pragma once
#include <cstdint>

struct MeshFileHeader
{
    char magic[4];
    uint32_t version;
    uint32_t vertex_count;
    uint32_t index_count;   
    uint32_t vertex_stride;
    uint32_t index_stride;
    uint32_t reserved[2];
};

static constexpr uint32_t kMeshMagic[4] = {'M', 'E', 'S', 'H'};
static constexpr uint32_t kMeshVersion = 1;

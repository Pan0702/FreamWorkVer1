#pragma once
#include <cstdint>
#include <vector>

struct NavigationContourVertex
{
    uint32_t x;
    uint32_t z;
    uint32_t height;
    uint32_t neighbor_region_id = 0;
};

struct NavigationContour
{
    uint32_t region_id;
    std::vector<NavigationContourVertex> vertices;
};

struct NavigationContourTriangle
{
    uint32_t vertex_indices[3] = {};
};

struct NavigationContourPolygon
{
    std::vector<uint32_t> vertex_indices;
};
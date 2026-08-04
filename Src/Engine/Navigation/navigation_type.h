#pragma once
#include <cstdint>
#include <vector>

#include "../../Core/Math/vec3.h"

struct NavigationMeshPolygon
{
    std::vector<uint32_t> vertex_indices;
    std::vector<uint32_t> neighbor_polygon_indices;
    uint32_t region_id = 0;
};

struct NavigationMeshData
{
    std::vector<Vec3> vertices;
    std::vector<NavigationMeshPolygon> polygons;
};

struct NavigationDetailMeshData
{
    std::vector<Vec3> vertices;
    std::vector<uint32_t> indices;
};
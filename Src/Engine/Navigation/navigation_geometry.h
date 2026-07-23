#pragma once
#include <vector>

#include "../../Core/Math/intersect.h"
#include "../../Core/Math/my_math.h"
struct NavigationGeometry
{
    std::vector<Vec3> vertices;
    std::vector<uint32> indices;
    Mat world_mat;
    Box world_bounds;
};

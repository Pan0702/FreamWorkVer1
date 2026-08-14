#pragma once
#include <vector>

#include "../../Core/Math/intersect.h"
#include "../../Core/Math/my_math.h"

/**
 * @brief NavMesh の元になる地形 1 つ分の形状。
 *
 * 頂点はワールド変換前のローカル座標で持ち、world_mat と world_bounds が
 * ワールド空間での配置を表す。
 */
struct NavigationGeometry
{
    std::vector<Vec3> vertices;
    std::vector<uint32> indices;
    Mat world_mat;
    Box world_bounds;
};



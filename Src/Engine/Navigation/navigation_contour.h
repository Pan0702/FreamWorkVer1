#pragma once
#include <cstdint>
#include <vector>

/**
 * @brief 領域の輪郭を構成する頂点。
 *
 * 位置はセル格子座標で持つ。neighbor_region_id は、この頂点から始まる辺の
 * 向こう側にある領域を示し、外周など隣接がない場合は 0 になる。
 */
struct NavigationContourVertex
{
    uint32_t x;
    uint32_t z;
    uint32_t height;
    uint32_t neighbor_region_id = 0;
};

/**
 * @brief 1 つの領域を囲む輪郭。
 *
 * 頂点は XZ 平面で反時計回りに並ぶ。
 */
struct NavigationContour
{
    uint32_t region_id;
    std::vector<NavigationContourVertex> vertices;
};

/**
 * @brief 輪郭を三角形分割して得られる三角形 1 枚。
 *
 * 頂点は輪郭の頂点配列へのインデックスで参照する。
 */
struct NavigationContourTriangle
{
    uint32_t vertex_indices[3] = {};
};

/**
 * @brief 三角形をマージして作る凸ポリゴン。
 *
 * 頂点は輪郭の頂点配列へのインデックスで参照する。
 */
struct NavigationContourPolygon
{
    std::vector<uint32_t> vertex_indices;
};

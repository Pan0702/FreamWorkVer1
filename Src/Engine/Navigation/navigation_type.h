#pragma once
#include <cstdint>
#include <vector>

#include "../../Core/Math/vec3.h"

/**
 * @brief NavMesh を構成する凸ポリゴン 1 枚分のデータ。
 *
 * 頂点は XZ 平面で反時計回りに並ぶ。隣接情報は辺ごとに持ち、
 * neighbor_polygon_indices[i] は vertex_indices[i] から始まる辺の
 * 向こう側にあるポリゴンを指す。隣接がない辺は UINT32_MAX。
 */
struct NavigationMeshPolygon
{
    std::vector<uint32_t> vertex_indices;
    std::vector<uint32_t> neighbor_polygon_indices;
    uint32_t region_id = 0;
};

/**
 * @brief 経路探索に使う NavMesh 本体。
 *
 * ポリゴンは頂点配列へのインデックスで頂点を参照する。
 */
struct NavigationMeshData
{
    std::vector<Vec3> vertices;
    std::vector<NavigationMeshPolygon> polygons;
};

/**
 * @brief NavMesh の高さを地形に沿わせるための三角形メッシュ。
 *
 * 経路探索には使わず、地面の高さを求める用途とデバッグ描画に使う。
 */
struct NavigationDetailMeshData
{
    std::vector<Vec3> vertices;
    std::vector<uint32_t> indices;
};

#pragma once
#include <cstdint>

/**
 * @brief ボクセル化した地形の、1 セル内で連続する立体の区間。
 *
 * 高さはセル高さを単位とする格子座標で表す。
 */
struct NavigationSpan
{
    uint32_t min_height = 0;
    uint32_t max_height = 0;
    bool is_walk = false;
};

/**
 * @brief 歩行可能な床面だけを取り出した、隣接情報付きの区間。
 *
 * NavigationSpan が立体の占有区間を表すのに対し、こちらは床面 1 枚を表す。
 * connection_indices は 4 方向の隣接スパンを指し、隣接がない方向は UINT32_MAX。
 */
struct NavigationCompactSpan
{
    uint32_t floor_height = 0;
    uint32_t clearance_height = 0;
    uint32_t dis_to_wall = UINT32_MAX;
    uint32_t region_id = 0;
    uint32_t connection_indices[4] = {
        UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX
    };
    bool is_walk = true ;

};

/**
 * @brief 1 セルが持つ NavigationCompactSpan の範囲。
 *
 * スパン本体は 1 本の配列にまとめて持つため、その先頭位置と個数で範囲を示す。
 */
struct NavigationCompactCell
{
    uint32_t first_span_index = 0;
    uint32_t span_count = 0;
};

#pragma once
#include <cstdint>

/**
 * @brief セル格子上の矩形範囲。
 *
 * 三角形が覆うセルの走査範囲を表すのに使う。
 */
struct CellRange
{
    uint32_t min_width_cell;
    uint32_t min_depth_cell;
    uint32_t max_depth_cell;
    uint32_t max_width_cell;
};

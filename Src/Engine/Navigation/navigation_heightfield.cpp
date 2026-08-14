#include "navigation_heightfield.h"
#include <cmath>
#include <algorithm>

bool NavigationHeightfield::Initialize(const Box& aabb, float size, float height)
{
    if (size <= 0.0f || height <= 0.0f)
    {
        return false;
    }
    if (aabb.max.x <= aabb.min.x || aabb.max.y <= aabb.min.y
        || aabb.max.z <= aabb.min.z)
    {
        return false;
    }

    world_bounds_ = aabb;
    cell_size_ = size;
    cell_height_ = height;

    // 端が欠けないよう切り上げる。範囲がセルの整数倍でない場合、
    // 切り捨てると最後の列が丸ごと失われる。
    const float width = world_bounds_.max.x - world_bounds_.min.x;
    width_ = static_cast<uint32>(std::ceil(width / cell_size_));

    const float depth = world_bounds_.max.z - world_bounds_.min.z;
    depth_ = static_cast<uint32>(std::ceil(depth / cell_size_));

    const uint32 cell_count = width_ * depth_;
    cells_.clear();
    cells_.resize(cell_count);
    return true;
}

NavigationHeightfieldCell* NavigationHeightfield::GetCell(uint32 x, uint32 z)
{
    if (z >= depth_ || x >= width_)
    {
        return nullptr;
    }
    // セルはZ行を単位とした1次元配列に並べている。
    const uint32 index = z * width_ + x;
    return &cells_[index];
}

bool NavigationHeightfield::AddSpan(uint32 x, uint32 z, const NavigationSpan& span)
{
    if (span.max_height < span.min_height)
    {
        return false;
    }
    const auto height_cell = GetCell(x, z);
    if (height_cell == nullptr)
    {
        return false;
    }
    // ここでは並びも重なりも気にせず積むだけ。整理は MergeSpans が行う。
    height_cell->spans.push_back(span);
    return true;
}

void NavigationHeightfield::MergeSpans()
{
    for (auto& cell : cells_)
    {
        if (cell.spans.size() <= 1)
        {
            continue;
        }
        // 低い順に並べることで、隣接判定を前から1回なめるだけで済ませる。
        (std::ranges::sort)(cell.spans,
                            [](NavigationSpan a, NavigationSpan b)
                            {
                                if (a.min_height != b.min_height)
                                {
                                    return a.min_height < b.min_height;
                                }
                                return a.max_height < b.max_height;
                            });

        std::vector<NavigationSpan> sorted_spans;
        NavigationSpan current = cell.spans.front();
        for (size_t i = 1; i < cell.spans.size(); i++)
        {
            NavigationSpan next = cell.spans[i];
            // 次のSpanと隣接してるか確認。
            // +1しているのは、1セル分の間しかない2つの区間も地続きとみなすため。
            if (current.max_height + 1 >= next.min_height)
            {
                // 隣接してるSpanのほうが大きかったら最大を更新。
                // 歩行可否は上面のものが残る。床は区間の上面だから。
                if (current.max_height < next.max_height)
                {
                    current.max_height = next.max_height;
                    current.is_walk = next.is_walk;
                }
                // 上面の高さが同じ場合は、どちらかが歩けるなら歩けるとする。
                if (current.max_height == next.max_height)
                {
                    current.is_walk |= next.is_walk;
                }
            }
            else
            {
                sorted_spans.push_back(current);
                current = next;
            }
        }
        // ループ内では書き出されない最後の1つを積む。
        sorted_spans.push_back(current);
        cell.spans = sorted_spans;
    }
}

const Box& NavigationHeightfield::GetWorldBounds() const
{
    return world_bounds_;
}

float NavigationHeightfield::GetCellSize() const
{
    return cell_size_;
}

float NavigationHeightfield::GetCellHeight() const
{
    return cell_height_;
}

uint32 NavigationHeightfield::GetWidth() const
{
    return width_;
}

uint32 NavigationHeightfield::GetDepth() const
{
    return depth_;
}

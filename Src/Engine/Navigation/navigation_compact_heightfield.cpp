#include "navigation_compact_heightfield.h"

#include "navigation_heightfield.h"

bool NavigationCompactHeightfield::Initialize(const NavigationHeightfield& source_heightfield)
{
    // セルの配置は元のハイトフィールドと揃える。両者の格子座標を
    // そのまま行き来できるようにするため。
    width_ = source_heightfield.GetWidth();
    depth_ = source_heightfield.GetDepth();
    cell_height_ = source_heightfield.GetCellHeight();
    cell_size_ = source_heightfield.GetCellSize();
    world_bounds_ = source_heightfield.GetWorldBounds();
    cells_.clear();
    cells_.resize(width_ * depth_);
    spans_.clear();
    return true;
}

bool NavigationCompactHeightfield::AddSpan(uint32 x, uint32 z, const NavigationCompactSpan& span)
{
    if (x >= width_ || z >= depth_)
    {
        return false;
    }
    const uint32 index = z * width_ + x;
    // セルは配列の範囲としてスパンを指すだけなので、同じセルのスパンは
    // 連続して追加しなければならない。
    if (cells_[index].span_count == 0)
    {
        cells_[index].first_span_index = spans_.size();
    }
    cells_[index].span_count++;
    spans_.push_back(span);
    return true;
}

NavigationCompactCell* NavigationCompactHeightfield::GetCell(uint32 x, uint32 z)
{
    if (x >= width_ || z >= depth_)
    {
        return nullptr;
    }
    // セルはZ行を単位とした1次元配列に並べている。
    const uint32 index = z * width_ + x;
    return &cells_[index];
}

const NavigationCompactCell* NavigationCompactHeightfield::GetCell(uint32 x, uint32 z) const
{
    if (x >= width_ || z >= depth_)
    {
        return nullptr;
    }

    const uint32 index = z * width_ + x;
    return &cells_[index];
}

NavigationCompactSpan* NavigationCompactHeightfield::GetSpan(uint32 span_index)
{
    if (span_index >= spans_.size())
    {
        return nullptr;
    }
    return &spans_[span_index];
}

const NavigationCompactSpan* NavigationCompactHeightfield::GetSpan(uint32 span_index)  const
{
    if (span_index >= spans_.size())
    {
        return nullptr;
    }
    return &spans_[span_index];
}


float NavigationCompactHeightfield::GetCellHeight() const
{
    return cell_height_;
}

uint32 NavigationCompactHeightfield::GetWidth() const
{
    return width_;
}

uint32 NavigationCompactHeightfield::GetDepth() const
{
    return depth_;
}
float NavigationCompactHeightfield::GetCellSize() const
{
    return cell_size_;
}

const Box& NavigationCompactHeightfield::GetWorldBounds() const
{
    return world_bounds_;
}

const std::vector<NavigationCompactSpan>& NavigationCompactHeightfield::GetSpans() const
{
    return spans_;
}

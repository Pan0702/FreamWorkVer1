#pragma once
#include <vector>

#include "navigation_heightfield_cell.h"
#include "../../Core/Math/intersect.h"


class NavigationHeightfield
{
public:
    bool Initialize(const Box& aabb, float size, float height);
    NavigationHeightfieldCell* GetCell(uint32 x, uint32 z);
    bool AddSpan(uint32 x, uint32 z, const NavigationSpan& span);
    void MergeSpans();
    const Box& GetWorldBounds() const;
    float GetCellSize() const;
    float GetCellHeight() const;
    uint32 GetWidth() const;
    uint32 GetDepth() const;
private:
    Box world_bounds_ = {};
    float cell_size_ = 0.0f;
    float cell_height_= 0.0f;
    uint32 width_ = 0;
    uint32 depth_ = 0;
    std::vector<NavigationHeightfieldCell> cells_;
};

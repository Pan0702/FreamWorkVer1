#pragma once
#include <vector>
#include "navigation_span.h"
#include "../../Core/Math/intersect.h"

class NavigationHeightfield;

class NavigationCompactHeightfield
{
public:
    bool Initialize(const NavigationHeightfield& source_heightfield);
    bool AddSpan(uint32 x, uint32 z, const NavigationCompactSpan& span);
    NavigationCompactCell* GetCell(uint32 x, uint32 z);
    NavigationCompactSpan* GetSpan(uint32 span_index);
    const NavigationCompactSpan* GetSpan(uint32 span_index) const;
    float GetCellHeight() const;
    uint32 GetWidth() const;
    uint32 GetDepth() const;
    float GetCellSize() const;
    const Box& GetWorldBounds() const;
    const std::vector<NavigationCompactSpan>& GetSpans() const;
private:
    std::vector<NavigationCompactCell> cells_;
    std::vector<NavigationCompactSpan> spans_;
    Box world_bounds_;
    float cell_size_;
    float cell_height_;
    uint32 width_;
    uint32 depth_;
};



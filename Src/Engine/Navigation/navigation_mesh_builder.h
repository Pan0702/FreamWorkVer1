#pragma once
#include "navigation_config.h"
#include <vector>

#include "navigation_geometry.h"
#include "../../Core/Math/intersect.h"
#include "navigation_cell_range.h"
#include "navigation_span.h"

class NavigationHeightfield;

class NavigationMeshBuilder
{
public:
    bool Build(const std::vector<NavigationGeometry>& geometries, const NavigationConfig& config,
               NavigationHeightfield& heightfield);
    bool RasterizeGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                           NavigationHeightfield& height) const;
    bool RasterizeTriangle(const Triangle& tri, const NavigationConfig& config, NavigationHeightfield& height) const;

private:
    bool CreateSpanFromHeightRange(float min_y, float max_y, const NavigationHeightfield& height,
                                   bool is_walk, NavigationSpan& span) const;

    bool IsWalkableTriangle(const Triangle& tri, const NavigationConfig& config) const;
    Triangle GetWorldTriangle(const NavigationGeometry& geometry, uint32 begin) const;
    Box CalcTriangleBounds(const Triangle& tri) const;
    bool CalcCellRange(const Box& b, const NavigationHeightfield& height, CellRange& range) const;
    Box CalcCellBounds(const NavigationHeightfield& height, uint32 x, uint32 z) const;
    std::vector<Vec3> ClipTriangleToCell(const Triangle& tri, const Box& b) const;
    std::vector<Vec3> ClipPolygonAgainstMinX(const std::vector<Vec3>& vertices, float min_x) const;
    std::vector<Vec3> ClipPolygonAgainstMaxX(const std::vector<Vec3>& vertices, float max_x) const;
    std::vector<Vec3> ClipPolygonAgainstMinZ(const std::vector<Vec3>& vertices, float min_z) const;
    std::vector<Vec3> ClipPolygonAgainstMaxZ(const std::vector<Vec3>& vertices, float max_z) const;
    bool CalcPolygonHeightRange(const std::vector<Vec3>& vertices, float& low_height, float& high_height) const;
};

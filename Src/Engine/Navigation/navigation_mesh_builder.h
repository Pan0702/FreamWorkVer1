#pragma once
#include "navigation_config.h"
#include <vector>

#include "navigation_geometry.h"
#include "../../Core/Math/intersect.h"
#include "navigation_cell_range.h"
#include "navigation_contour.h"
#include "navigation_span.h"
#include "navigation_type.h"

class NavigationHeightfield;
class NavigationCompactHeightfield;

class NavigationMeshBuilder
{
public:
    bool Build(const std::vector<NavigationGeometry>& geometries, const NavigationConfig& config,
               NavigationHeightfield& heightfield);
    bool RasterizeGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                           NavigationHeightfield& height) const;
    bool RasterizeTriangle(const Triangle& tri, const NavigationConfig& config, NavigationHeightfield& height) const;

    bool BuildCompactHeightfield(NavigationHeightfield* source, NavigationCompactHeightfield& output,
                                 const NavigationConfig& config);
    bool BuildRegions(NavigationCompactHeightfield& heightfield, const NavigationConfig& config) const;
    bool BuildContours(NavigationCompactHeightfield& heightfield, const NavigationConfig& config,
                       std::vector<NavigationContour>& contours) const;
    bool BuildNavigationMeshData(const NavigationCompactHeightfield& heightfield,
                                 const std::vector<NavigationContour>& contours,
                                 const NavigationConfig& config, NavigationMeshData& mesh_data) const;

private:
    bool BuildPolygonAdjacency(NavigationMeshData& mesh_data) const;
    uint32 FindOrAddNavigationMeshVertex(const NavigationContourVertex& contour_vertex,
                                         const NavigationCompactHeightfield& heightfield,
                                         NavigationMeshData& mesh_data) const;
    Vec3 ConvertContourVertexToWorld(const NavigationContourVertex& vertex,
                                     const NavigationCompactHeightfield& heightfield) const;
    bool BuildContourPolygons(const NavigationContour& contour, uint32 max_vertex_count,
                              std::vector<NavigationContourPolygon>& out_poly) const;
    bool TryMergeContourPolygons(const NavigationContour& contour, const NavigationContourPolygon& fir,
                                 const NavigationContourPolygon& sec, uint32 max_vertex_count,
                                 NavigationContourPolygon& out_merged) const;
    bool IsContourPolygonConvex(const NavigationContour& contour, const NavigationContourPolygon& polygon) const;
    bool FindSharedPolygonEdge(const NavigationContourPolygon& fir, const NavigationContourPolygon& sec,
                               uint32& out_fir_index, uint32& out_sec_index) const;
    bool TriangulateContour(const NavigationContour& contour, std::vector<NavigationContourTriangle>& triangles) const;
    bool IsContourEar(const NavigationContour& contour, const std::vector<uint32>& remaining_indices,
                      uint32 remaining_position) const;
    bool IsPointInsideOrOnTriangleXZ(const NavigationContourVertex& point, const NavigationContourVertex& a,
                                     const NavigationContourVertex& b, const NavigationContourVertex& c) const;
    bool MergeContourHoles(std::vector<NavigationContour>& contours) const;
    bool FindContainingOuterContour(const NavigationContour& hole, const std::vector<NavigationContour>& contours,
                                    uint32& outer_index) const;
    bool MergeHoleIntoContour(NavigationContour& outer, const NavigationContour& hole,
                              uint32 outer_index, uint32 hole_index) const;
    bool FindHoleBridge(const NavigationContour& outer, const NavigationContour& hole_contour,
                        uint32& out_outer_index, uint32& out_hole_index) const;
    bool IsHoleBridgeVisible(const NavigationContour& outer, uint32 outer_index,
                             const NavigationContour& hole_contour, uint32 hole_index) const;
    bool DoSegmentsIntersectXZ(const NavigationContourVertex& a, const NavigationContourVertex& b,
                               const NavigationContourVertex& c, const NavigationContourVertex& d) const;
    bool IsPointOnSegmentXZ(const NavigationContourVertex& point, const NavigationContourVertex& start,
                            const NavigationContourVertex& end) const;
    int64 CalcTriangleSignedAreaTwiceXZ(const NavigationContourVertex& a, const NavigationContourVertex& b,
                                        const NavigationContourVertex& c) const;
    bool IsPointInsideContour(const NavigationContourVertex& point, const NavigationContour& contour) const;
    int64 CalcContourSignedAreaTwice(const NavigationContour& contour) const;
    bool SimplifyContour(const NavigationContour& raw_contour, float max_error_in_cells,
                         NavigationContour& simplified_contour) const;
    float CalcPointToSegmentDistanceSquared(const NavigationContourVertex& point, const NavigationContourVertex& start,
                                            const NavigationContourVertex& end) const;
    uint32 CalcContourCornerHeight(const NavigationCompactHeightfield& heightfield, uint32 span_index,
                                   uint32 direction) const;
    bool TraceRegionContour(const NavigationCompactHeightfield& heightfield, uint32 start_x, uint32 start_z,
                            uint32 start_span_index, uint32 start_direction, std::vector<uint8>& boundary_masks,
                            NavigationContour& contour) const;
    std::vector<uint8> BuildContourBoundaryMasks(NavigationCompactHeightfield& heightfield) const;
    void MergeSmallRegions(NavigationCompactHeightfield& heightfield, uint32 next_id,
                           const NavigationConfig& config) const;
    void FilterSmallRegions(NavigationCompactHeightfield& heightfield, uint32 next_region_id,
                            const NavigationConfig& config) const;
    void ExpandRegionsAtLevel(NavigationCompactHeightfield& compact_heightfield, uint32 min_dist) const;
    void FloodNewRegionsAtLevel(NavigationCompactHeightfield& heightfield, uint32 min_dist,
                                uint32& next_region_id) const;
    void ErodeWalkableArea(NavigationCompactHeightfield& heightfield, const NavigationConfig& config);
    bool BuildCompactConnections(NavigationCompactHeightfield& heightfield, const NavigationConfig& config);
    void FilterLedgeSpans(NavigationHeightfield& heightfield, const NavigationConfig& config);
    void FilterLowCeilingSpans(NavigationHeightfield& heightfield, const NavigationConfig& config);
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

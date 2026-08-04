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
    bool BuildNavigationDetailMesh(const NavigationCompactHeightfield& heightfield, const NavigationMeshData& mesh_data,
                                   const NavigationConfig& config, NavigationDetailMeshData& detail_mesh_data) const;

private:
    struct SolidIntersection
    {
        float height = 0.0f;
        int32 depth_delta = 0;
        bool is_walkable_top = false;
    };
    void AppendUniformDetailTriangle(
        const NavigationCompactHeightfield& heightfield,const NavigationConfig& config,
        const Vec3& a,const Vec3& b,const Vec3& c,uint32 region_id,NavigationDetailMeshData& detail_mesh_data) const;
    float CalcDetailTriangleMaxPenetration(
        const NavigationCompactHeightfield& heightfield, const NavigationConfig& config,
        const Vec3& a, const Vec3& b, const Vec3& c, uint32 region_id,
        Vec3& out_position) const;
    bool TryGetClosestSpanFloorHeight(const NavigationCompactHeightfield& heightfield, int32 cell_x, int32 cell_z,
                                      uint32 region_id, float reference_height, float& out_height) const;
    void AppendAdaptiveDetailTriangle(const NavigationCompactHeightfield& heightfield, const NavigationConfig& config,
                                      const Vec3& a, const Vec3& b, const Vec3& c, uint32 region_id,
                                      uint32 subdivision_depth, NavigationDetailMeshData& detail_mesh_data) const;
    bool TrySampleSurfaceHeight(const NavigationCompactHeightfield& heightfield,
                                float world_x, float world_z, uint32 region_id, float reference_height, float& out_height, float max_height_diff) const;
    bool RasterizeSolidGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                                NavigationHeightfield& heightfield) const;
    bool RasterizeSolidCell(uint32 x, uint32 z, std::vector<SolidIntersection>& intersections,
                            NavigationHeightfield& heightfield) const;
    bool CollectSolidIntersections(const NavigationGeometry& geometry, const NavigationConfig& config,
                                   const NavigationHeightfield& heightfield,
                                   std::vector<std::vector<SolidIntersection>>& out_cell_intersections) const;
    bool TryCalcVerticalIntersectionHeight(const Triangle& triangle, float sample_x, float sample_z,
                                           float& out_height) const;
    bool IsClosedGeometry(const NavigationGeometry& geometry) const;
    void FilterUnreachableRegions(NavigationCompactHeightfield& heightfield) const;
    float CalcSurfaceCornerHeight(const NavigationCompactHeightfield& heightfield, int32 corner_x, int32 corner_z,
                                  uint32 region_id, float ref_height, float max_height_diff) const;
    float CalcPolygonAreaXZ(const std::vector<Vec3>& vertices) const;
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
                         float max_height_error_in_cells, float max_edge_len,
                         NavigationContour& simplified_contour) const;
    float CalcPointToSegmentDistanceSquared(const NavigationContourVertex& point, const NavigationContourVertex& start,
                                            const NavigationContourVertex& end) const;
    float CalcPointToSegmentHeightError(const NavigationContourVertex& point,
                                        const NavigationContourVertex& start,
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

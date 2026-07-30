#include "navigation_mesh_builder.h"
#include <algorithm>
#include <queue>

#include "navigation_config.h"
#include "navigation_geometry.h"
#include "navigation_heightfield.h"
#include "navigation_compact_heightfield.h"

Box NavigationMeshBuilder::CalcCellBounds(const NavigationHeightfield& height, uint32 x, uint32 z) const
{
    Box b;
    b.min.x = height.GetWorldBounds().min.x + x * height.GetCellSize();
    b.max.x = (std::min)(b.min.x + height.GetCellSize(), height.GetWorldBounds().max.x);

    b.min.y = height.GetWorldBounds().min.y;
    b.max.y = height.GetWorldBounds().max.y;

    b.min.z = height.GetWorldBounds().min.z + z * height.GetCellSize();
    b.max.z = (std::min)(b.min.z + height.GetCellSize(), height.GetWorldBounds().max.z);
    return b;
}

std::vector<Vec3> NavigationMeshBuilder::ClipTriangleToCell(const Triangle& tri, const Box& b) const
{
    std::vector<Vec3> result = {tri.a, tri.b, tri.c};
    result = ClipPolygonAgainstMinX(result, b.min.x);
    result = ClipPolygonAgainstMaxX(result, b.max.x);
    result = ClipPolygonAgainstMinZ(result, b.min.z);
    result = ClipPolygonAgainstMaxZ(result, b.max.z);
    return result;
}

bool NavigationMeshBuilder::Build(const std::vector<NavigationGeometry>& geometries, const NavigationConfig& config,
                                  NavigationHeightfield& heightfield)
{
    bool success_flag = false;
    for (auto& geometry : geometries)
    {
        success_flag |= RasterizeGeometry(geometry, config, heightfield);
    }
    heightfield.MergeSpans();
    FilterLowCeilingSpans(heightfield, config);
    FilterLedgeSpans(heightfield, config);
    return success_flag;
}

bool NavigationMeshBuilder::RasterizeGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                                              NavigationHeightfield& height) const
{
    bool success_flag = false;
    for (uint32 i = 0; i < geometry.indices.size(); i += 3)
    {
        const Triangle tri = GetWorldTriangle(geometry, i);
        success_flag |= RasterizeTriangle(tri, config, height);
    }
    return success_flag;
}

bool NavigationMeshBuilder::RasterizeTriangle(const Triangle& tri, const NavigationConfig& config,
                                              NavigationHeightfield& height) const
{
    Box b = CalcTriangleBounds(tri);
    CellRange range;
    if (!CalcCellRange(b, height, range))
    {
        return false;
    }
    const bool is_walk = IsWalkableTriangle(tri, config);
    bool is_push_back = false;
    for (int32 z = range.min_depth_cell; z <= range.max_depth_cell; ++z)
    {
        for (int32 x = range.min_width_cell; x <= range.max_width_cell; ++x)
        {
            const Box cell_b = CalcCellBounds(height, x, z);
            const std::vector<Vec3> clipped_vertices = ClipTriangleToCell(tri, cell_b);
            if (clipped_vertices.empty())
            {
                continue;
            }
            float min_y, max_y;
            if (!CalcPolygonHeightRange(clipped_vertices, min_y, max_y))
            {
                continue;
            }
            NavigationSpan span;
            if (!CreateSpanFromHeightRange(min_y, max_y, height, is_walk, span))
            {
                continue;
            }

            if (height.AddSpan(x, z, span))
            {
                is_push_back = true;
            }
        }
    }
    return is_push_back;
}

bool NavigationMeshBuilder::BuildCompactHeightfield(NavigationHeightfield* source,
                                                    NavigationCompactHeightfield& output,
                                                    const NavigationConfig& config)
{
    output.Initialize(*source);
    for (int x = 0; x < source->GetWidth(); ++x)
    {
        for (int z = 0; z < source->GetDepth(); ++z)
        {
            auto spans = source->GetCell(x, z);
            for (int s = 0; s < spans->spans.size(); ++s)
            {
                const auto& span = spans->spans[s];
                if (!span.is_walk)continue;
                NavigationCompactSpan compact_span;
                compact_span.clearance_height = s + 1 < spans->spans.size()
                                                    ? spans->spans[s + 1].min_height - span.max_height
                                                    : UINT32_MAX;
                compact_span.floor_height = span.max_height;

                output.AddSpan(x, z, compact_span);
            }
        }
    }
    if (!BuildCompactConnections(output, config))
    {
        return false;
    }
    ErodeWalkableArea(output, config);
    return BuildRegions(output, config);
}

bool NavigationMeshBuilder::BuildRegions(NavigationCompactHeightfield& heightfield,
                                         const NavigationConfig& config) const
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());
    uint32 max_dist = 0;
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);
        span->region_id = 0;
        if (!span->is_walk)
        {
            continue;
        }
        max_dist = (std::max)(span->dis_to_wall, max_dist);
    }
    uint32 next_region_id = 1;
    for (int32 min = static_cast<int32>(max_dist); min >= 0; min--)
    {
        ExpandRegionsAtLevel(heightfield, min);
        FloodNewRegionsAtLevel(heightfield, min, next_region_id);
    }
    MergeSmallRegions(heightfield, next_region_id, config);
    FilterSmallRegions(heightfield, next_region_id, config);
    return next_region_id > 1;
}

bool NavigationMeshBuilder::BuildContours(NavigationCompactHeightfield& heightfield, const NavigationConfig& config,
                                          std::vector<NavigationContour>& contours) const
{
    contours.clear();
    std::vector<uint8> boundary_masks = BuildContourBoundaryMasks(heightfield);
    const float max_error_in_cells = config.max_contour_simplification_error / heightfield.GetCellSize();
    for (int x = 0; x < heightfield.GetWidth(); ++x)
    {
        for (int z = 0; z < heightfield.GetDepth(); ++z)
        {
            auto* cell = heightfield.GetCell(x, z);
            for (int i = 0; i < cell->span_count; ++i)
            {
                const uint32 span_index = cell->first_span_index + i;
                auto* span = heightfield.GetSpan(span_index);
                if (span->region_id == 0 || !span->is_walk || boundary_masks[span_index] == 0)
                {
                    continue;
                }
                for (auto direction = 0; direction < 4; ++direction)
                {
                    const uint8 edge_bit = static_cast<uint8>(1u << direction);
                    uint8 bit = boundary_masks[span_index] & edge_bit;
                    if (bit != 0)
                    {
                        uint32 start_dir = direction;
                        NavigationContour contour;
                        if (TraceRegionContour(heightfield, x, z, span_index, start_dir, boundary_masks, contour))
                        {
                            NavigationContour simplified_contour;
                            if (SimplifyContour(contour, max_error_in_cells, simplified_contour))
                            {
                                contours.push_back(simplified_contour);
                            }
                        }
                    }
                }
            }
        }
    }
    return MergeContourHoles(contours);
}

bool NavigationMeshBuilder::BuildNavigationMeshData(const NavigationCompactHeightfield& heightfield,
                                                    const std::vector<NavigationContour>& contours,
                                                    const NavigationConfig& config, NavigationMeshData& mesh_data) const
{
    if (contours.empty() || config.max_vertex_per_poly < 3)
    {
        return false;
    }

    NavigationMeshData generated_mesh_data;

    for (const auto& contour : contours)
    {
        std::vector<NavigationContourPolygon> polygons;
        if (!BuildContourPolygons(contour, config.max_vertex_per_poly, polygons))
        {
            return false;
        }

        for (const auto& polygon : polygons)
        {
            NavigationMeshPolygon mesh_polygon;
            mesh_polygon.region_id = contour.region_id;
            mesh_polygon.vertex_indices.reserve(polygon.vertex_indices.size());

            for (uint32 contour_vertex_index : polygon.vertex_indices)
            {
                if (contour_vertex_index >= contour.vertices.size())
                {
                    return false;
                }

                const uint32 mesh_vertex_index = FindOrAddNavigationMeshVertex(
                    contour.vertices[contour_vertex_index], heightfield, generated_mesh_data);
                mesh_polygon.vertex_indices.push_back(mesh_vertex_index);
            }
            generated_mesh_data.polygons.push_back(mesh_polygon);
        }
    }

    if (generated_mesh_data.polygons.empty() || generated_mesh_data.vertices.empty())
    {
        return false;
    }

    if (!BuildPolygonAdjacency(generated_mesh_data))
    {
        return false;
    }
    
    mesh_data.vertices.swap(generated_mesh_data.vertices);
    mesh_data.polygons.swap(generated_mesh_data.polygons);
    return true;
}

bool NavigationMeshBuilder::BuildPolygonAdjacency(NavigationMeshData& mesh_data) const
{
    const uint32 polygon_count = static_cast<uint32>(mesh_data.polygons.size());
    if (polygon_count == 0)
    {
        return false;
    }

    for (NavigationMeshPolygon& polygon : mesh_data.polygons)
    {
        const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
        
        if (vertex_count < 3)
        {
            return false;
        }
        
        for (uint32 vertex_index : polygon.vertex_indices)
        {
            if (vertex_index >= mesh_data.vertices.size())
            {
                return false;
            }
        }
        
        polygon.neighbor_polygon_indices.assign(vertex_count, UINT32_MAX);
    }
    
    for (uint32 fir_index = 0; fir_index < polygon_count; ++fir_index)
    {
         NavigationMeshPolygon& fir = mesh_data.polygons[fir_index];
        const uint32 fir_vertex_count = static_cast<uint32>(fir.vertex_indices.size());
        
        for (uint32 sec_index = fir_index + 1; sec_index < polygon_count; ++sec_index)
        {
             NavigationMeshPolygon& sec = mesh_data.polygons[sec_index];
            const uint32 sec_vertex_count = static_cast<uint32>(sec.vertex_indices.size());
            uint32 shared_edge_count = 0;
            for (uint32 fir_edge = 0; fir_edge < fir_vertex_count; ++fir_edge)
            {
                const uint32 fir_vertex_index_next = (fir_edge + 1) % fir_vertex_count;
                 const uint32 fir_start = fir.vertex_indices[fir_edge];
                 const uint32 fir_end = fir.vertex_indices[fir_vertex_index_next];
                 for (uint32 sec_vertex_index = 0; sec_vertex_index < sec_vertex_count; ++sec_vertex_index)
                 {
                     const uint32 sec_vertex_index_next = (sec_vertex_index + 1) % sec_vertex_count;
                     const uint32 sec_start = sec.vertex_indices[sec_vertex_index];
                     const uint32 sec_end = sec.vertex_indices[sec_vertex_index_next];
                     if (fir_start != sec_end || fir_end != sec_start)
                     {
                         continue;
                     }
                     
                     if (fir.neighbor_polygon_indices[fir_edge] != UINT32_MAX ||
                         sec.neighbor_polygon_indices[sec_vertex_index] != UINT32_MAX)
                     {
                         return false;
                     }
                     shared_edge_count++;
                     
                     fir.neighbor_polygon_indices[fir_edge] = sec_index;
                     sec.neighbor_polygon_indices[sec_vertex_index] = fir_index;
                     if (shared_edge_count > 1)
                     {
                         return false;
                     }
                 }
            }
        }
    }
    return true;
}

uint32 NavigationMeshBuilder::FindOrAddNavigationMeshVertex(const NavigationContourVertex& contour_vertex,
                                                            const NavigationCompactHeightfield& heightfield,
                                                            NavigationMeshData& mesh_data) const
{
    const Vec3 world_pos = ConvertContourVertexToWorld(contour_vertex, heightfield);

    for (uint32 vertex_index = 0; vertex_index < mesh_data.vertices.size(); ++vertex_index)
    {
        const auto& vertex = mesh_data.vertices[vertex_index];

        if (vertex == world_pos)
        {
            return vertex_index;
        }
    }
    const uint32 new_vertex_index = static_cast<uint32>(mesh_data.vertices.size());
    mesh_data.vertices.push_back(world_pos);
    return new_vertex_index;
}


Vec3 NavigationMeshBuilder::ConvertContourVertexToWorld(const NavigationContourVertex& vertex,
                                                        const NavigationCompactHeightfield& heightfield) const
{
    const Box& box = heightfield.GetWorldBounds();
    Vec3 world_pos = {};
    world_pos.x = static_cast<float>(vertex.x) * heightfield.GetCellSize() + box.min.x;
    world_pos.y = static_cast<float>(vertex.height) * heightfield.GetCellHeight() + box.min.y;
    world_pos.z = static_cast<float>(vertex.z) * heightfield.GetCellSize() + box.min.z;
    return world_pos;
}

bool NavigationMeshBuilder::BuildContourPolygons(const NavigationContour& contour, uint32 max_vertex_count,
                                                 std::vector<NavigationContourPolygon>& out_poly) const
{
    if (max_vertex_count < 3)
    {
        return false;
    }

    std::vector<NavigationContourTriangle> triangles;

    if (!TriangulateContour(contour, triangles))
    {
        return false;
    }

    std::vector<NavigationContourPolygon> polygons;
    polygons.reserve(triangles.size());

    for (const NavigationContourTriangle& triangle : triangles)
    {
        NavigationContourPolygon polygon;

        polygon.vertex_indices.push_back(
            triangle.vertex_indices[0]);
        polygon.vertex_indices.push_back(
            triangle.vertex_indices[1]);
        polygon.vertex_indices.push_back(
            triangle.vertex_indices[2]);

        polygons.push_back(polygon);
    }

    bool merged_any = true;
    while (merged_any)
    {
        merged_any = false;
        for (uint32 fir_index = 0; fir_index < polygons.size() && !merged_any; ++fir_index)
        {
            for (uint32 sec_index = fir_index + 1; sec_index < polygons.size(); ++sec_index)
            {
                NavigationContourPolygon merge_poly;
                if (!TryMergeContourPolygons(contour, polygons[fir_index], polygons[sec_index], max_vertex_count,
                                             merge_poly))
                {
                    continue;
                }

                polygons[fir_index].vertex_indices.swap(merge_poly.vertex_indices);
                polygons.erase(polygons.begin() + sec_index);

                merged_any = true;
                break;
            }
        }
    }

    if (polygons.empty())
    {
        return false;
    }
    out_poly.swap(polygons);
    return true;
}

bool NavigationMeshBuilder::TryMergeContourPolygons(const NavigationContour& contour,
                                                    const NavigationContourPolygon& fir,
                                                    const NavigationContourPolygon& sec, uint32 max_vertex_count,
                                                    NavigationContourPolygon& out_merged) const
{
    uint32 fir_edge = 0;
    uint32 sec_edge = 0;

    if (!FindSharedPolygonEdge(fir, sec, fir_edge, sec_edge))
    {
        return false;
    }

    const uint32 fir_count = static_cast<uint32>(fir.vertex_indices.size());
    const uint32 sec_count = static_cast<uint32>(sec.vertex_indices.size());

    const uint32 merged_count = fir_count + sec_count - 2;

    if (max_vertex_count < 3 || merged_count > max_vertex_count)
    {
        return false;
    }
    NavigationContourPolygon merged;
    merged.vertex_indices.reserve(merged_count);
    for (uint32 offset = 0; offset < fir_count; ++offset)
    {
        const uint32 index = (fir_edge + 1 + offset) % fir_count;
        merged.vertex_indices.push_back(fir.vertex_indices[index]);
    }

    for (uint32 offset = 2; offset < sec_count; ++offset)
    {
        const uint32 index = (sec_edge + offset) % sec_count;
        merged.vertex_indices.push_back(sec.vertex_indices[index]);
    }

    if (!IsContourPolygonConvex(contour, merged))
    {
        return false;
    }
    out_merged.vertex_indices.swap(merged.vertex_indices);
    return true;
}

bool NavigationMeshBuilder::IsContourPolygonConvex(const NavigationContour& contour,
                                                   const NavigationContourPolygon& polygon) const
{
    const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
    if (vertex_count < 3)
    {
        return false;
    }

    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 fir_index = polygon.vertex_indices[i];
        if (fir_index >= contour.vertices.size())
        {
            return false;
        }
        const auto& fir_vertex = contour.vertices[fir_index];
        for (uint32 j = i + 1; j < vertex_count; ++j)
        {
            const uint32 sec_index = polygon.vertex_indices[j];
            if (sec_index >= contour.vertices.size())
            {
                return false;
            }
            const auto& sec_vertex = contour.vertices[sec_index];

            if (fir_vertex.x == sec_vertex.x && fir_vertex.z == sec_vertex.z)
            {
                return false;
            }
        }
    }
    bool has_positive = false;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 next = (1 + i) % vertex_count;
        const uint32 next_next = (2 + i) % vertex_count;
        const auto& fir_index = contour.vertices[polygon.vertex_indices[i]];
        const auto& sec_index = contour.vertices[polygon.vertex_indices[next]];
        const auto& sec_next_index = contour.vertices[polygon.vertex_indices[next_next]];
        const int64 turn = CalcTriangleSignedAreaTwiceXZ(fir_index, sec_index, sec_next_index);

        if (turn < 0)
        {
            return false;
        }
        if (turn > 0)
        {
            has_positive = true;
        }
    }
    return has_positive;
}

bool NavigationMeshBuilder::FindSharedPolygonEdge(const NavigationContourPolygon& fir,
                                                  const NavigationContourPolygon& sec, uint32& out_fir_index,
                                                  uint32& out_sec_index) const
{
    const uint32 fir_count = static_cast<uint32>(fir.vertex_indices.size());
    const uint32 sec_count = static_cast<uint32>(sec.vertex_indices.size());
    if (fir_count < 3 || sec_count < 3)
    {
        return false;
    }

    for (uint32 fir_index = 0; fir_index < fir_count; ++fir_index)
    {
        const uint32 fir_next_index = (fir_index + 1) % fir_count;
        const uint32 fir_start = fir.vertex_indices[fir_index];
        const uint32 fir_end = fir.vertex_indices[fir_next_index];
        for (uint32 sec_index = 0; sec_index < sec_count; ++sec_index)
        {
            const uint32 sec_next_index = (sec_index + 1) % sec_count;
            const uint32 sec_start = sec.vertex_indices[sec_index];
            const uint32 sec_end = sec.vertex_indices[sec_next_index];
            if (fir_start == sec_end && fir_end == sec_start)
            {
                out_fir_index = fir_index;
                out_sec_index = sec_index;
                return true;
            }
        }
    }
    return false;
}

bool NavigationMeshBuilder::TriangulateContour(const NavigationContour& contour,
                                               std::vector<NavigationContourTriangle>& triangles) const
{
    const uint32 vertex_count = static_cast<uint32>(contour.vertices.size());

    if (vertex_count < 3 || CalcContourSignedAreaTwice(contour) <= 0)
    {
        return false;
    }

    std::vector<uint32> indices;
    indices.reserve(vertex_count);
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        indices.push_back(i);
    }
    std::vector<NavigationContourTriangle> generated_triangles;
    generated_triangles.reserve(vertex_count - 2);
    while (indices.size() > 3)
    {
        bool ear_found = false;

        const uint32 remaining_count = static_cast<uint32>(indices.size());
        for (uint32 pos = 0; pos < remaining_count; ++pos)
        {
            if (!IsContourEar(contour, indices, pos))
            {
                continue;
            }
            const uint32 prev_pos = (pos + remaining_count - 1) % remaining_count;
            const uint32 next_pos = (pos + 1) % remaining_count;
            NavigationContourTriangle triangle;
            triangle.vertex_indices[0] = indices[prev_pos];
            triangle.vertex_indices[1] = indices[pos];
            triangle.vertex_indices[2] = indices[next_pos];

            generated_triangles.push_back(triangle);
            indices.erase(indices.begin() + pos);
            ear_found = true;
            break;
        }

        //切り取れる角がなかったら
        if (!ear_found)
        {
            return false;
        }
    }

    const auto& a = contour.vertices[indices[0]];
    const auto& b = contour.vertices[indices[1]];
    const auto& c = contour.vertices[indices[2]];
    if (CalcTriangleSignedAreaTwiceXZ(a, b, c) <= 0)
    {
        return false;
    }

    NavigationContourTriangle final_triangle;
    final_triangle.vertex_indices[0] = indices[0];
    final_triangle.vertex_indices[1] = indices[1];
    final_triangle.vertex_indices[2] = indices[2];

    generated_triangles.push_back(final_triangle);
    triangles.swap(generated_triangles);
    return true;
}

bool NavigationMeshBuilder::IsContourEar(const NavigationContour& contour, const std::vector<uint32>& remaining_indices,
                                         uint32 remaining_position) const
{
    const uint32 remaining_count = static_cast<uint32>(remaining_indices.size());
    if (remaining_count < 3 || remaining_position >= remaining_count)
    {
        return false;
    }

    const uint32 prev_pos = (remaining_position + remaining_count - 1) % remaining_count;
    const uint32 next_pos = (remaining_position + 1) % remaining_count;

    const uint32 prev_index = remaining_indices[prev_pos];
    const uint32 cur_index = remaining_indices[remaining_position];
    const uint32 next_index = remaining_indices[next_pos];

    if (prev_index >= contour.vertices.size() || cur_index >= contour.vertices.size() || next_index >= contour.vertices.
        size())
    {
        return false;
    }

    const auto& prev_vertex = contour.vertices[prev_index];
    const auto& cur_vertex = contour.vertices[cur_index];
    const auto& next_vertex = contour.vertices[next_index];

    if (CalcTriangleSignedAreaTwiceXZ(prev_vertex, cur_vertex, next_vertex) <= 0)
    {
        return false;
    }
    for (uint32 edge = 0; edge < remaining_count; ++edge)
    {
        const uint32 edge_next_pos = (edge + 1) % remaining_count;
        const uint32 edge_start_index = remaining_indices[edge];
        const uint32 edge_end_index = remaining_indices[edge_next_pos];

        if (edge_start_index >= contour.vertices.size() || edge_end_index >= contour.vertices.size())
        {
            return false;
        }

        const auto& edge_start = contour.vertices[edge_start_index];
        const auto& edge_end = contour.vertices[edge_end_index];

        const bool share_endpoint = (edge_start.x == prev_vertex.x && edge_start.z == prev_vertex.z) ||
            (edge_end.x == prev_vertex.x && edge_end.z == prev_vertex.z) ||
            (edge_start.x == next_vertex.x && edge_start.z == next_vertex.z) ||
            (edge_end.x == next_vertex.x && edge_end.z == next_vertex.z);

        if (share_endpoint)
        {
            continue;
        }
        if (DoSegmentsIntersectXZ(prev_vertex, next_vertex, edge_start, edge_end))
        {
            return false;
        }
    }
    for (uint32 vertex_index : remaining_indices)
    {
        if (vertex_index >= contour.vertices.size())
        {
            return false;
        }
        if (vertex_index == prev_index || vertex_index == cur_index || vertex_index == next_index)
        {
            continue;
        }

        const auto& vertex = contour.vertices[vertex_index];
        const bool same_as_triangle = (vertex.x == prev_vertex.x && vertex.z == prev_vertex.z) ||
            (vertex.x == cur_vertex.x && vertex.z == cur_vertex.z) ||
            (vertex.x == next_vertex.x && vertex.z == next_vertex.z);
        if (same_as_triangle)
        {
            continue;
        }

        if (IsPointInsideOrOnTriangleXZ(vertex, prev_vertex, cur_vertex, next_vertex))
        {
            return false;
        }
    }
    return true;
}

bool NavigationMeshBuilder::IsPointInsideOrOnTriangleXZ(const NavigationContourVertex& point,
                                                        const NavigationContourVertex& a,
                                                        const NavigationContourVertex& b,
                                                        const NavigationContourVertex& c) const
{
    const int64 abp = CalcTriangleSignedAreaTwiceXZ(a, b, point);
    const int64 bcp = CalcTriangleSignedAreaTwiceXZ(b, c, point);
    const int64 cap = CalcTriangleSignedAreaTwiceXZ(c, a, point);

    const bool has_negative = (abp < 0) || (bcp < 0) || (cap < 0);
    const bool has_positive = (abp > 0) || (bcp > 0) || (cap > 0);
    return !(has_negative && has_positive);
}

bool NavigationMeshBuilder::MergeContourHoles(std::vector<NavigationContour>& contours) const
{
    std::vector<NavigationContour> outer_contours;
    std::vector<NavigationContour> hole_contours;

    for (auto& contour : contours)
    {
        if (contour.vertices.size() < 3)
        {
            continue;
        }

        const int64 area_twice = CalcContourSignedAreaTwice(contour);
        if (area_twice > 0)
        {
            outer_contours.push_back(contour);
        }
        else if (area_twice < 0)
        {
            hole_contours.push_back(contour);
        }
    }
    if (outer_contours.empty())
    {
        return false;
    }
    for (const auto& hole : hole_contours)
    {
        uint32 containing_outer_index = 0;
        if (!FindContainingOuterContour(hole, outer_contours, containing_outer_index))
        {
            return false;
        }
        NavigationContour& outer = outer_contours[containing_outer_index];

        uint32 bridge_outer_index = 0;
        uint32 bridge_hole_index = 0;

        if (!FindHoleBridge(outer, hole, bridge_outer_index, bridge_hole_index))
        {
            return false;
        }

        if (!MergeHoleIntoContour(outer, hole, bridge_outer_index, bridge_hole_index))
        {
            return false;
        }
    }
    contours.swap(outer_contours);
    return !contours.empty();
}

bool NavigationMeshBuilder::FindContainingOuterContour(const NavigationContour& hole,
                                                       const std::vector<NavigationContour>& contours,
                                                       uint32& outer_index) const
{
    if (hole.vertices.size() < 3)
    {
        return false;
    }

    bool found = false;
    int64 best_area_twice = 0;
    for (uint32 i = 0; i < contours.size(); ++i)
    {
        const NavigationContour& candidate = contours[i];
        if (candidate.vertices.size() < 3 ||
            candidate.region_id != hole.region_id)
        {
            continue;
        }
        const int64 area_twice = CalcContourSignedAreaTwice(candidate);
        if (area_twice <= 0)
        {
            continue;
        }
        if (!IsPointInsideContour(hole.vertices[0], candidate))
        {
            continue;
        }

        if (!found || area_twice < best_area_twice)
        {
            found = true;
            best_area_twice = area_twice;
            outer_index = i;
        }
    }
    return found;
}

bool NavigationMeshBuilder::MergeHoleIntoContour(NavigationContour& outer, const NavigationContour& hole,
                                                 uint32 outer_index, uint32 hole_index) const
{
    if (outer.vertices.size() < 3 ||
        hole.vertices.size() < 3 ||
        outer_index >= outer.vertices.size() ||
        hole_index >= hole.vertices.size())
    {
        return false;
    }

    const uint32 outer_count = static_cast<uint32>(outer.vertices.size());
    const uint32 hole_count = static_cast<uint32>(hole.vertices.size());
    std::vector<NavigationContourVertex> merge_vertices;

    for (uint32 i = 0; i <= outer_index; i++)
    {
        merge_vertices.push_back(outer.vertices[i]);
    }
    for (uint32 i = 0; i < hole_count; i++)
    {
        const uint32 index = (hole_index + i) % hole_count;
        merge_vertices.push_back(hole.vertices[index]);
    }

    merge_vertices.push_back(hole.vertices[hole_index]);
    merge_vertices.push_back(outer.vertices[outer_index]);

    for (uint32 i = outer_index + 1; i < outer_count; i++)
    {
        merge_vertices.push_back(outer.vertices[i]);
    }
    outer.vertices.swap(merge_vertices);
    return true;
}

bool NavigationMeshBuilder::FindHoleBridge(const NavigationContour& outer, const NavigationContour& hole_contour,
                                           uint32& out_outer_index, uint32& out_hole_index) const
{
    if (outer.vertices.size() < 3 || hole_contour.vertices.size() < 3)
    {
        return false;
    }
    bool found = false;
    double best_dis_sq = 0.0;
    for (uint32 outer_index = 0; outer_index < outer.vertices.size(); ++outer_index)
    {
        for (uint32 hole_index = 0; hole_index < hole_contour.vertices.size(); hole_index++)
        {
            if (!IsHoleBridgeVisible(outer, outer_index, hole_contour, hole_index))
            {
                continue;
            }
            const auto& outer_vertex = outer.vertices[outer_index];
            const auto& hole_vertex = hole_contour.vertices[hole_index];

            const double diff_x = static_cast<double>(outer_vertex.x) - static_cast<double>(hole_vertex.x);
            const double diff_z = static_cast<double>(outer_vertex.z) - static_cast<double>(hole_vertex.z);
            const double dis_sq = diff_x * diff_x + diff_z * diff_z;
            if (!found || dis_sq < best_dis_sq)
            {
                found = true;
                best_dis_sq = dis_sq;
                out_outer_index = outer_index;
                out_hole_index = hole_index;
            }
        }
    }
    return found;
}

bool NavigationMeshBuilder::IsHoleBridgeVisible(const NavigationContour& outer, uint32 outer_index,
                                                const NavigationContour& hole_contour, uint32 hole_index) const
{
    if (outer.vertices.size() < 3 ||
        hole_contour.vertices.size() < 3 ||
        outer_index >= outer.vertices.size() ||
        hole_index >= hole_contour.vertices.size())
    {
        return false;
    }
    const auto& outer_vertex = outer.vertices[outer_index];
    const auto& hole_vertex = hole_contour.vertices[hole_index];

    for (uint32 i = 0; i < outer.vertices.size(); ++i)
    {
        const uint32 next_index = (i + 1) % static_cast<uint32>(outer.vertices.size());
        if (i == outer_index || next_index == outer_index)
        {
            continue;
        }
        if (DoSegmentsIntersectXZ(outer_vertex, hole_vertex,
                                  outer.vertices[i], outer.vertices[next_index]))
        {
            return false;
        }
    }
    for (uint32 i = 0; i < hole_contour.vertices.size(); ++i)
    {
        const uint32 next_index = (i + 1) % static_cast<uint32>(hole_contour.vertices.size());
        if (i == hole_index || next_index == hole_index)
        {
            continue;
        }
        if (DoSegmentsIntersectXZ(outer_vertex, hole_vertex,
                                  hole_contour.vertices[i], hole_contour.vertices[next_index]))
        {
            return false;
        }
    }
    return true;
}

bool NavigationMeshBuilder::DoSegmentsIntersectXZ(const NavigationContourVertex& a, const NavigationContourVertex& b,
                                                  const NavigationContourVertex& c,
                                                  const NavigationContourVertex& d) const
{
    const int64 abc = CalcTriangleSignedAreaTwiceXZ(a, b, c);
    const int64 abd = CalcTriangleSignedAreaTwiceXZ(a, b, d);
    const int64 cda = CalcTriangleSignedAreaTwiceXZ(c, d, a);
    const int64 cdb = CalcTriangleSignedAreaTwiceXZ(c, d, b);
    const bool c_d_opposite = (abc > 0 && abd < 0) || (abc < 0 && abd > 0);
    const bool a_b_opposite = (cda > 0 && cdb < 0) || (cda < 0 && cdb > 0);
    if (a_b_opposite && c_d_opposite)
    {
        return true;
    }
    if (abc == 0 && IsPointOnSegmentXZ(c, a, b))
    {
        return true;
    }
    if (abd == 0 && IsPointOnSegmentXZ(d, a, b))
    {
        return true;
    }
    if (cda == 0 && IsPointOnSegmentXZ(a, c, d))
    {
        return true;
    }
    if (cdb == 0 && IsPointOnSegmentXZ(b, c, d))
    {
        return true;
    }
    return false;
}

bool NavigationMeshBuilder::IsPointOnSegmentXZ(const NavigationContourVertex& point,
                                               const NavigationContourVertex& start,
                                               const NavigationContourVertex& end) const
{
    if (CalcTriangleSignedAreaTwiceXZ(point, start, end) != 0)
    {
        return false;
    }
    const auto min_x = (std::min)(start.x, end.x);
    const auto max_x = (std::max)(start.x, end.x);
    const auto min_z = (std::min)(start.z, end.z);
    const auto max_z = (std::max)(start.z, end.z);

    return point.x >= min_x && point.x <= max_x && point.z >= min_z && point.z <= max_z;
}

int64 NavigationMeshBuilder::CalcTriangleSignedAreaTwiceXZ(const NavigationContourVertex& a,
                                                           const NavigationContourVertex& b,
                                                           const NavigationContourVertex& c) const
{
    const int64 ab_x = static_cast<int64>(b.x) - static_cast<int64>(a.x);
    const int64 ab_z = static_cast<int64>(b.z) - static_cast<int64>(a.z);
    const int64 ac_x = static_cast<int64>(c.x) - static_cast<int64>(a.x);
    const int64 ac_z = static_cast<int64>(c.z) - static_cast<int64>(a.z);
    return ab_x * ac_z - ab_z * ac_x;
}

bool NavigationMeshBuilder::IsPointInsideContour(const NavigationContourVertex& point,
                                                 const NavigationContour& contour) const
{
    uint32 vertex_count = static_cast<uint32>(contour.vertices.size());
    if (vertex_count < 3)
    {
        return false;
    }
    bool inside = false;
    const auto& vertices = contour.vertices;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 next_index = (i + 1) % vertex_count;
        const double point_x = static_cast<double>(point.x);
        const double point_z = static_cast<double>(point.z);

        const double current_x = static_cast<double>(vertices[i].x);
        const double current_z = static_cast<double>(vertices[i].z);

        const double next_x = static_cast<double>(vertices[next_index].x);
        const double next_z = static_cast<double>(vertices[next_index].z);
        const bool cross_z = (current_z > point_z) != (next_z > point_z);
        if (!cross_z)
        {
            continue;
        }
        const double intersection_x = current_x + (next_x - current_x) *
            (point_z - current_z) / (next_z - current_z);
        if (intersection_x > point_x)
        {
            inside = !inside;
        }
    }
    return inside;
}

int64 NavigationMeshBuilder::CalcContourSignedAreaTwice(const NavigationContour& contour) const
{
    uint32 vertex_count = static_cast<uint32>(contour.vertices.size());
    if (vertex_count < 3)
    {
        return 0;
    }
    int64 area_twice = 0;
    const auto& vertices = contour.vertices;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 next_index = (i + 1) % vertex_count;
        area_twice += static_cast<int64>(vertices[i].x) * static_cast<int64>(vertices[next_index].z)
            - static_cast<int64>(vertices[next_index].x) * static_cast<int64>(vertices[i].z);
    }
    return area_twice;
}

bool NavigationMeshBuilder::SimplifyContour(const NavigationContour& raw_contour, float max_error_in_cells,
                                            NavigationContour& simplified_contour) const
{
    uint32 vertex_count = static_cast<uint32>(raw_contour.vertices.size());
    std::vector<uint8> keep_flags(vertex_count, 0);
    uint32 kept_count = 0;
    if (vertex_count < 3)
    {
        return false;
    }
    simplified_contour.vertices.clear();
    simplified_contour.region_id = raw_contour.region_id;
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 prev_index = (i + vertex_count - 1) % vertex_count;
        const NavigationContourVertex& vertex = raw_contour.vertices[i];
        const NavigationContourVertex& prev_vertex = raw_contour.vertices[prev_index];
        if (vertex.neighbor_region_id != prev_vertex.neighbor_region_id)
        {
            keep_flags[i] = 1;
            ++kept_count;
        }
    }
    if (kept_count < 2)
    {
        uint32 min_idx = 0;
        uint32 max_idx = 0;
        for (uint32 i = 0; i < vertex_count; ++i)
        {
            if (raw_contour.vertices[i].x < raw_contour.vertices[min_idx].x)
            {
                min_idx = i;
            }
            if (raw_contour.vertices[i].x == raw_contour.vertices[min_idx].x &&
                raw_contour.vertices[i].z < raw_contour.vertices[min_idx].z)
            {
                min_idx = i;
            }
            if (raw_contour.vertices[i].x > raw_contour.vertices[max_idx].x)
            {
                max_idx = i;
            }
            if (raw_contour.vertices[i].x == raw_contour.vertices[max_idx].x &&
                raw_contour.vertices[i].z > raw_contour.vertices[max_idx].z)
            {
                max_idx = i;
            }
        }
        if (keep_flags[min_idx] == 0)
        {
            keep_flags[min_idx] = 1;
            ++kept_count;
        }
        if (keep_flags[max_idx] == 0)
        {
            keep_flags[max_idx] = 1;
            ++kept_count;
        }
    }
    if (kept_count < 2)
    {
        return false;
    }
    if (kept_count == 2)
    {
        int32 first_kept_index = -1;
        int32 second_kept_index = -1;
        float far_sq_dis = 0.0f;
        int32 far_idx = -1;
        for (uint32 i = 0; i < keep_flags.size(); ++i)
        {
            if (keep_flags[i] == 0)
            {
                continue;
            }
            if (keep_flags[i] == 1 && first_kept_index < 0)
            {
                first_kept_index = static_cast<int32>(i);
            }
            else
            {
                second_kept_index = static_cast<int32>(i);
            }
        }
        for (uint32 i = 0; i < keep_flags.size(); ++i)
        {
            if (keep_flags[i] != 0)
            {
                continue;
            }

            float sq_dis = CalcPointToSegmentDistanceSquared(raw_contour.vertices[i],
                                                             raw_contour.vertices[first_kept_index],
                                                             raw_contour.vertices[second_kept_index]);
            if (sq_dis > far_sq_dis)
            {
                far_idx = static_cast<int32>(i);
                far_sq_dis = sq_dis;
            }
        }
        if (far_idx < 0 || far_sq_dis <= 0.0f)
        {
            return false;
        }
        keep_flags[far_idx] = 1;
        ++kept_count;
    }
    const float max_error_in_cells_sq = max_error_in_cells * max_error_in_cells;
    bool add_vertex = false;
    do
    {
        add_vertex = false;
        std::vector<uint32> kept_indices;
        for (uint32 i = 0; i < keep_flags.size(); ++i)
        {
            if (keep_flags[i] == 1)
            {
                kept_indices.push_back(i);
            }
        }
        float max_far = 0.0f;
        int32 max_far_index = -1;
        for (uint32 i = 0; i < kept_indices.size(); ++i)
        {
            const uint32 start_index = kept_indices[i];
            const uint32 end_index = kept_indices[(i + 1) % kept_indices.size()];
            uint32 raw_index = (start_index + 1) % vertex_count;
            while (raw_index != end_index)
            {
                const float sq_dis = CalcPointToSegmentDistanceSquared(raw_contour.vertices[raw_index],
                                                                       raw_contour.vertices[start_index],
                                                                       raw_contour.vertices[end_index]);
                if (sq_dis > max_far)
                {
                    max_far = sq_dis;
                    max_far_index = static_cast<int32>(raw_index);
                }
                raw_index = (raw_index + 1) % vertex_count;
            }
        }


        if (max_far > max_error_in_cells_sq && max_far_index >= 0)
        {
            add_vertex = true;
            kept_count++;
            keep_flags[max_far_index] = 1;
        }
    }
    while (add_vertex);
    for (int i = 0; i < keep_flags.size(); ++i)
    {
        if (keep_flags[i] == 1)
        {
            simplified_contour.vertices.push_back(raw_contour.vertices[i]);
        }
    }
    return simplified_contour.vertices.size() >= 3;
}

float NavigationMeshBuilder::CalcPointToSegmentDistanceSquared(const NavigationContourVertex& point,
                                                               const NavigationContourVertex& start,
                                                               const NavigationContourVertex& end) const
{
    const float se_dir_x = static_cast<float>(end.x) - static_cast<float>(start.x);
    const float se_dir_z = static_cast<float>(end.z) - static_cast<float>(start.z);
    const float sp_dir_x = static_cast<float>(point.x) - static_cast<float>(start.x);
    const float sp_dir_z = static_cast<float>(point.z) - static_cast<float>(start.z);
    const float dot_se_sp = se_dir_x * sp_dir_x + se_dir_z * sp_dir_z;
    const float dot_se_se = se_dir_x * se_dir_x + se_dir_z * se_dir_z;
    if (dot_se_se == 0.0f)
    {
        return sp_dir_x * sp_dir_x + sp_dir_z * sp_dir_z;
    }
    float t = dot_se_sp / dot_se_se;
    t = std::clamp(t, 0.0f, 1.0f);
    const float closest_x = static_cast<float>(start.x) + static_cast<float>(se_dir_x) * t;
    const float closest_z = static_cast<float>(start.z) + static_cast<float>(se_dir_z) * t;
    const float distance_x = closest_x - static_cast<float>(point.x);

    const float distance_z = closest_z - static_cast<float>(point.z);

    return distance_x * distance_x +
        distance_z * distance_z;
}

uint32 NavigationMeshBuilder::CalcContourCornerHeight(const NavigationCompactHeightfield& heightfield,
                                                      uint32 span_index,
                                                      uint32 direction) const
{
    const auto* span = heightfield.GetSpan(span_index);
    uint32 max_floor_height = span->floor_height;
    uint32 prev_dir = (direction + 3) % 4;
    const uint32 neighbor_index = span->connection_indices[direction];
    uint32 diagonal_index = UINT32_MAX;
    if (neighbor_index != UINT32_MAX)
    {
        const auto* neighbor_span = heightfield.GetSpan(neighbor_index);
        max_floor_height = (std::max)(max_floor_height, neighbor_span->floor_height);
        diagonal_index = neighbor_span->connection_indices[prev_dir];
    }
    const uint32 prev_neighbor_index = span->connection_indices[prev_dir];
    if (prev_neighbor_index != UINT32_MAX)
    {
        const auto* prev_neighbor_span = heightfield.GetSpan(prev_neighbor_index);
        max_floor_height = (std::max)(max_floor_height, prev_neighbor_span->floor_height);
        if (diagonal_index == UINT32_MAX)
        {
            diagonal_index = prev_neighbor_span->connection_indices[direction];
        }
    }

    if (diagonal_index != UINT32_MAX)
    {
        const auto* diagonal_span = heightfield.GetSpan(diagonal_index);
        max_floor_height = (std::max)(max_floor_height, diagonal_span->floor_height);
    }
    return max_floor_height;
}

bool NavigationMeshBuilder::TraceRegionContour(const NavigationCompactHeightfield& heightfield, uint32 start_x,
                                               uint32 start_z, uint32 start_span_index, uint32 start_direction,
                                               std::vector<uint8>& boundary_masks,
                                               NavigationContour& contour) const
{
    contour.vertices.clear();
    uint32 x = start_x;
    uint32 z = start_z;
    uint32 span_index = start_span_index;
    uint32 direction = start_direction;
    auto span = heightfield.GetSpan(start_span_index);
    contour.region_id = span->region_id;
    do
    {
        const uint8 edge_bit = static_cast<uint8>(1u << direction);
        const bool is_boundary = (boundary_masks[span_index] & edge_bit) != 0;
        const auto* span = heightfield.GetSpan(span_index);

        const uint32 neighbor_index = span->connection_indices[direction];
        if (is_boundary)
        {
            uint32 vertex_x = x;
            uint32 vertex_z = z;
            switch (edge_bit)
            {
            case 1:
                vertex_x = x + 1;
                break;
            case 2:
                vertex_z = z + 1;
                vertex_x = x + 1;
                break;
            case 4:
                vertex_z = z + 1;
                break;
            case 8:
                vertex_z = z;
                vertex_x = x;
                break;
            default:
                break;
            }

            const uint32 height = CalcContourCornerHeight(heightfield, span_index, direction);
            uint32 neighbor_region_id = 0;
            if (neighbor_index != UINT32_MAX)
            {
                const auto* neighbor_span = heightfield.GetSpan(neighbor_index);

                if (neighbor_span->is_walk && neighbor_span->region_id != span->region_id)
                {
                    neighbor_region_id = neighbor_span->region_id;
                }
            }
            NavigationContourVertex vertex;
            vertex = {vertex_x, vertex_z, height, neighbor_region_id};
            contour.vertices.push_back(vertex);
            boundary_masks[span_index] &= static_cast<uint8>(~edge_bit);
            direction = (direction + 1) % 4;
        }
        else
        {
            span_index = neighbor_index;

            switch (direction)
            {
            case 0:
                x++;
                break;
            case 1:
                z++;
                break;
            case 2:
                x--;
                break;
            case 3:
                z--;
                break;
            }
            direction = (direction + 3) % 4;
        }
    }
    while (x != start_x ||
        z != start_z ||
        span_index != start_span_index ||
        direction != start_direction);
    return contour.vertices.size() >= 3;
}

std::vector<uint8> NavigationMeshBuilder::BuildContourBoundaryMasks(NavigationCompactHeightfield& heightfield) const
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());
    std::vector<uint8> masks(span_count, 0);

    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);
        if (!span->is_walk || span->region_id == 0)
        {
            continue;
        }
        for (uint32 direction = 0; direction < 4; ++direction)
        {
            uint32 neighbor_index = span->connection_indices[direction];
            if (neighbor_index == UINT32_MAX)
            {
                masks[span_index] |= static_cast<uint8>(1u << direction);
                continue;
            }
            auto* neighbor_span = heightfield.GetSpan(neighbor_index);
            if (!neighbor_span->is_walk || neighbor_span->region_id != span->region_id)
            {
                masks[span_index] |= static_cast<uint8>(1u << direction);
                continue;
            }
        }
    }
    return masks;
}


void NavigationMeshBuilder::MergeSmallRegions(NavigationCompactHeightfield& heightfield, uint32 next_id,
                                              const NavigationConfig& config) const
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());

    std::vector<uint32> region_span_counts(next_id, 0);

    // RegionごとのSpan数を数える。
    for (uint32 span_index = 0;
         span_index < span_count;
         ++span_index)
    {
        const auto* span = heightfield.GetSpan(span_index);

        if (!span->is_walk || span->region_id == 0)
        {
            continue;
        }

        ++region_span_counts[span->region_id];
    }

    std::vector<uint32> merge_targets(next_id, 0);
    auto& spans = heightfield.GetSpans();
    for (int i = 0; i < spans.size(); ++i)
    {
        NavigationCompactSpan* span = heightfield.GetSpan(i);
        if (!span->is_walk || span->region_id == 0)
        {
            continue;
        }

        if (region_span_counts[span->region_id] >= config.merge_region_span_count)
        {
            continue;
        }

        for (unsigned int neighbor_index : span->connection_indices)
        {
            if (neighbor_index == UINT32_MAX)
            {
                continue;
            }
            NavigationCompactSpan* neighbor_span = heightfield.GetSpan(neighbor_index);
            if (!neighbor_span->is_walk || neighbor_span->region_id == 0)
            {
                continue;
            }

            if (span->region_id == neighbor_span->region_id)
            {
                continue;
            }
            if (region_span_counts[neighbor_span->region_id] < region_span_counts[span->region_id])
            {
                continue;
            }
            if (region_span_counts[neighbor_span->region_id] == region_span_counts[span->region_id]
                && neighbor_span->region_id > span->region_id)
            {
                continue;
            }
            merge_targets[span->region_id] = neighbor_span->region_id;
        }
    }
    for (int i = 0; i < spans.size(); ++i)
    {
        auto span = heightfield.GetSpan(i);
        const uint32 target = merge_targets[span->region_id];
        if (target != 0 && target != span->region_id)
        {
            span->region_id = target;
        }
    }
}

void NavigationMeshBuilder::FilterSmallRegions(NavigationCompactHeightfield& heightfield, uint32 next_region_id,
                                               const NavigationConfig& config) const
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());

    std::vector<uint32> region_span_counts(next_region_id, 0);

    // RegionごとのSpan数を数える。
    for (uint32 span_index = 0;
         span_index < span_count;
         ++span_index)
    {
        const auto* span = heightfield.GetSpan(span_index);

        if (!span->is_walk || span->region_id == 0)
        {
            continue;
        }

        ++region_span_counts[span->region_id];
    }

    // 小さすぎるRegionを歩行不可にする。
    for (uint32 span_index = 0;
         span_index < span_count;
         ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);

        if (!span->is_walk || span->region_id == 0)
        {
            continue;
        }

        if (region_span_counts[span->region_id] <
            config.min_region_span_count)
        {
            span->is_walk = false;
            span->region_id = 0;
        }
    }

    // 歩行不可Spanにつながる接続を解除する。
    for (uint32 span_index = 0;
         span_index < span_count;
         ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);

        for (uint32 direction = 0;
             direction < 4;
             ++direction)
        {
            uint32& neighbor_index =
                span->connection_indices[direction];

            if (!span->is_walk)
            {
                neighbor_index = UINT32_MAX;
                continue;
            }

            if (neighbor_index == UINT32_MAX)
            {
                continue;
            }

            const auto* neighbor_span =
                heightfield.GetSpan(neighbor_index);

            if (!neighbor_span->is_walk)
            {
                neighbor_index = UINT32_MAX;
            }
        }
    }
}


void NavigationMeshBuilder::ExpandRegionsAtLevel(NavigationCompactHeightfield& heightfield, uint32 min_dist) const
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());
    std::queue<uint32> span_queue;
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);
        if (span->is_walk && span->region_id != 0)
        {
            span_queue.push(span_index);
        }
    }
    while (!span_queue.empty())
    {
        const uint32 span_index = span_queue.front();
        span_queue.pop();
        auto* span = heightfield.GetSpan(span_index);
        for (uint32 neighbor_index : span->connection_indices)
        {
            if (neighbor_index == UINT32_MAX)
            {
                continue;
            }
            auto* neighbor_span = heightfield.GetSpan(neighbor_index);
            if (!neighbor_span->is_walk)
            {
                continue;
            }
            if (neighbor_span->region_id != 0)
            {
                continue;
            }
            if (neighbor_span->dis_to_wall < min_dist)
            {
                continue;
            }
            neighbor_span->region_id = span->region_id;
            span_queue.push(neighbor_index);
        }
    }
}

void NavigationMeshBuilder::FloodNewRegionsAtLevel(NavigationCompactHeightfield& heightfield, uint32 min_dist,
                                                   uint32& next_region_id) const
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());
    std::queue<uint32> span_queue;
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);

        if (!span->is_walk || span->region_id != 0 || span->dis_to_wall < min_dist)
        {
            continue;
        }
        span->region_id = next_region_id;
        span_queue.push(span_index);

        while (!span_queue.empty())
        {
            const uint32 span_index_queue = span_queue.front();
            span_queue.pop();
            auto* span = heightfield.GetSpan(span_index_queue);
            for (uint32 neighbor_index : span->connection_indices)
            {
                if (neighbor_index == UINT32_MAX)
                {
                    continue;
                }
                auto* neighbor_span = heightfield.GetSpan(neighbor_index);
                if (!neighbor_span->is_walk || neighbor_span->region_id != 0 || neighbor_span->dis_to_wall < min_dist)
                {
                    continue;
                }
                neighbor_span->region_id = span->region_id;
                span_queue.push(neighbor_index);
            }
        }
        next_region_id++;
    }
}

void NavigationMeshBuilder::ErodeWalkableArea(NavigationCompactHeightfield& heightfield, const NavigationConfig& config)
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());

    std::queue<uint32> span_queue;

    // 壁際のSpanを距離0として登録
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);
        span->dis_to_wall = UINT32_MAX;

        for (uint32 connection_index : span->connection_indices)
        {
            if (connection_index == UINT32_MAX)
            {
                span->dis_to_wall = 0;
                span_queue.push(span_index);
                break;
            }
        }
    }

    // 壁からの最短距離を接続先へ伝播
    while (!span_queue.empty())
    {
        const uint32 span_index = span_queue.front();
        span_queue.pop();

        auto* span = heightfield.GetSpan(span_index);
        const uint32 candidate_distance = span->dis_to_wall + 1;

        for (uint32 neighbor_index : span->connection_indices)
        {
            if (neighbor_index == UINT32_MAX)
            {
                continue;
            }

            auto* neighbor_span = heightfield.GetSpan(neighbor_index);

            if (candidate_distance < neighbor_span->dis_to_wall)
            {
                neighbor_span->dis_to_wall = candidate_distance;
                span_queue.push(neighbor_index);
            }
        }
    }

    const uint32 radius_in_cells = static_cast<uint32>(
        std::ceil(config.agent_radius / heightfield.GetCellSize()));

    // 半径内のSpanを歩行不可にする
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);

        if (span->dis_to_wall < radius_in_cells)
        {
            span->is_walk = false;
        }
    }

    // 歩行不可Spanにつながる接続を解除
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);

        for (unsigned int& neighbor_index : span->connection_indices)
        {
            if (!span->is_walk)
            {
                neighbor_index = UINT32_MAX;
                continue;
            }

            if (neighbor_index == UINT32_MAX)
            {
                continue;
            }

            const auto* neighbor_span = heightfield.GetSpan(neighbor_index);

            if (!neighbor_span->is_walk)
            {
                neighbor_index = UINT32_MAX;
            }
        }
    }
}

bool NavigationMeshBuilder::BuildCompactConnections(NavigationCompactHeightfield& heightfield,
                                                    const NavigationConfig& config)
{
    for (int x = 0; x < heightfield.GetWidth(); ++x)
    {
        for (int z = 0; z < heightfield.GetDepth(); ++z)
        {
            auto* cell = heightfield.GetCell(x, z);
            for (int i = 0; i < cell->span_count; i++)
            {
                const uint32 span_index = cell->first_span_index + i;
                NavigationCompactSpan* span = heightfield.GetSpan(span_index);
                const float current_floor = span->floor_height * heightfield.GetCellHeight();
                const float current_ceiling_height = span->clearance_height == UINT32_MAX
                                                         ? FLT_MAX
                                                         : span->clearance_height * heightfield.GetCellHeight() +
                                                         current_floor;
                for (int connect = 0; connect < 4; ++connect)
                {
                    int32 neighbor_x = x;
                    int32 neighbor_z = z;
                    switch (connect)
                    {
                    case 0:
                        neighbor_x += 1;
                        break;
                    case 1:
                        neighbor_z += 1;
                        break;
                    case 2:
                        neighbor_x -= 1;
                        break;
                    case 3:
                        neighbor_z -= 1;
                        break;
                    }
                    if (neighbor_x >= heightfield.GetWidth() || neighbor_z >= heightfield.GetDepth() ||
                        neighbor_x < 0 || neighbor_z < 0)
                    {
                        continue;
                    }
                    auto* neighbor_cell = heightfield.GetCell(neighbor_x, neighbor_z);
                    for (int n = 0; n < neighbor_cell->span_count; n++)
                    {
                        uint32 neighbor_span_index = neighbor_cell->first_span_index + n;
                        auto* neighbor_span = heightfield.GetSpan(neighbor_span_index);
                        const float neighbor_floor = neighbor_span->floor_height * heightfield.GetCellHeight();
                        const float neighbor_ceiling_height = (neighbor_span->clearance_height == UINT32_MAX)
                                                                  ? FLT_MAX
                                                                  : neighbor_span->clearance_height * heightfield.
                                                                  GetCellHeight() + neighbor_floor;

                        const float overlap_floor = (neighbor_floor > current_floor) ? neighbor_floor : current_floor;
                        const float overlap_ceiling = (neighbor_ceiling_height < current_ceiling_height)
                                                          ? neighbor_ceiling_height
                                                          : current_ceiling_height;

                        const float overlap_height = overlap_ceiling - overlap_floor;
                        if (overlap_height < config.agent_height)
                        {
                            continue;
                        }

                        const float abs_diff_floor = std::abs(neighbor_floor - current_floor);
                        if (abs_diff_floor <= config.agent_max_climb)
                        {
                            span->connection_indices[connect] = neighbor_span_index;
                            break;
                        }
                    }
                }
            }
        }
    }
    return true;
}

void NavigationMeshBuilder::FilterLedgeSpans(NavigationHeightfield& heightfield, const NavigationConfig& config)
{
    for (int x = 0; x < heightfield.GetWidth(); ++x)
    {
        for (int z = 0; z < heightfield.GetDepth(); ++z)
        {
            auto cell = heightfield.GetCell(x, z);
            for (int c = 0; c < cell->spans.size(); ++c)
            {
                if (!cell->spans[c].is_walk)
                {
                    continue;
                }
                const std::vector<Vec2> dir =
                    {Vec2(1, 0), Vec2(0, 1), Vec2(-1, 0), Vec2(0, -1)};


                for (const Vec2& d : dir)
                {
                    bool has_reachable_neighbor = false;
                    const int32 neighbor_x = d.x + x;
                    const int32 neighbor_z = d.y + z;
                    if (neighbor_x < 0 || neighbor_x >= heightfield.GetWidth() ||
                        neighbor_z < 0 || neighbor_z >= heightfield.GetDepth())
                    {
                        cell->spans[c].is_walk = false;
                        break;
                    }

                    auto neighbor_cell = heightfield.GetCell(neighbor_x, neighbor_z);
                    const float current_floor = cell->spans[c].max_height * heightfield.GetCellHeight();
                    const float current_ceiling_height = c + 1 < cell->spans.size()
                                                             ? cell->spans[c + 1].min_height * heightfield.
                                                             GetCellHeight()
                                                             : FLT_MAX;

                    for (int n = 0; n < neighbor_cell->spans.size(); ++n)
                    {
                        const float neighbor_floor = neighbor_cell->spans[n].max_height * heightfield.GetCellHeight();
                        const float neighbor_ceiling_height = (n + 1 < neighbor_cell->spans.size())
                                                                  ? neighbor_cell->spans[n + 1].min_height * heightfield
                                                                  .GetCellHeight()
                                                                  : FLT_MAX;

                        const float overlap_floor = (neighbor_floor > current_floor) ? neighbor_floor : current_floor;
                        const float overlap_ceiling = (neighbor_ceiling_height < current_ceiling_height)
                                                          ? neighbor_ceiling_height
                                                          : current_ceiling_height;

                        const float overlap_height = overlap_ceiling - overlap_floor;
                        if (overlap_height < config.agent_height)
                        {
                            continue;
                        }

                        const float abs_diff_floor = std::abs(neighbor_floor - current_floor);
                        if (abs_diff_floor <= config.agent_max_climb)
                        {
                            has_reachable_neighbor = true;
                            break;
                        }
                    }
                    if (!has_reachable_neighbor)
                    {
                        cell->spans[c].is_walk = false;
                        break;
                    }
                }
            }
        }
    }
}

void NavigationMeshBuilder::FilterLowCeilingSpans(NavigationHeightfield& heightfield, const NavigationConfig& config)
{
    for (int x = 0; x < heightfield.GetWidth(); ++x)
    {
        for (int z = 0; z < heightfield.GetDepth(); ++z)
        {
            auto cell = heightfield.GetCell(x, z);
            for (int c = 0; c + 1 < cell->spans.size(); ++c)
            {
                auto& span = cell->spans[c];
                auto& next_span = cell->spans[c + 1];
                const float height = (next_span.min_height - span.max_height) * heightfield.GetCellHeight();
                if (height < config.agent_height)
                {
                    span.is_walk = false;
                }
            }
        }
    }
}

bool NavigationMeshBuilder::CreateSpanFromHeightRange(float min_y, float max_y,
                                                      const NavigationHeightfield& height, bool is_walk,
                                                      NavigationSpan& span) const
{
    //低い床よりもMaxが高かったらfalse
    if (min_y > max_y)
    {
        return false;
    }
    //セルにスペースがなかったらfalse
    if (height.GetCellHeight() <= 0.0f)
    {
        return false;
    }
    //Yが範囲外ならfalse
    if (min_y > height.GetWorldBounds().max.y || max_y < height.GetWorldBounds().min.y)
    {
        return false;
    }

    float clamped_min_y = std::clamp(min_y, height.GetWorldBounds().min.y,
                                     height.GetWorldBounds().max.y);
    float clamped_max_y = std::clamp(max_y, height.GetWorldBounds().min.y,
                                     height.GetWorldBounds().max.y);
    float relative_min = (clamped_min_y - height.GetWorldBounds().min.y)
        / height.GetCellHeight();
    float relative_max = (clamped_max_y - height.GetWorldBounds().min.y)
        / height.GetCellHeight();

    span.min_height = static_cast<uint32>(std::floor(relative_min));
    span.max_height = static_cast<uint32>(std::ceil(relative_max));

    span.is_walk = is_walk;
    return true;
}

bool NavigationMeshBuilder::IsWalkableTriangle(const Triangle& tri, const NavigationConfig& config) const
{
    const Vec3 side_1 = tri.b - tri.a;
    const Vec3 side_2 = tri.c - tri.a;
    Vec3 normal = Cross(side_1, side_2);
    if (normal.LengthSquared() < kEpsilon)
    {
        return false;
    }
    normal.Normalize();
    const Vec3 kUp(0, 1, 0);
    float up_dot = Dot(normal, kUp);
    return up_dot >= std::cos(config.agent_max_slope_deg * kDegToRad);
}

Triangle NavigationMeshBuilder::GetWorldTriangle(const NavigationGeometry& geometry, uint32 begin) const
{
    Triangle result;
    uint32 index_a = geometry.indices[begin];
    uint32 index_b = geometry.indices[begin + 1];
    uint32 index_c = geometry.indices[begin + 2];

    result.a = TransformPoint(geometry.world_mat, geometry.vertices[index_a]);
    result.b = TransformPoint(geometry.world_mat, geometry.vertices[index_b]);
    result.c = TransformPoint(geometry.world_mat, geometry.vertices[index_c]);
    return result;
}

Box NavigationMeshBuilder::CalcTriangleBounds(const Triangle& tri) const
{
    Box result;
    result.max.x = (std::max)(tri.a.x, (std::max)(tri.b.x, tri.c.x));
    result.max.y = (std::max)(tri.a.y, (std::max)(tri.b.y, tri.c.y));
    result.max.z = (std::max)(tri.a.z, (std::max)(tri.b.z, tri.c.z));
    result.min.x = (std::min)(tri.a.x, (std::min)(tri.b.x, tri.c.x));
    result.min.y = (std::min)(tri.a.y, (std::min)(tri.b.y, tri.c.y));
    result.min.z = (std::min)(tri.a.z, (std::min)(tri.b.z, tri.c.z));
    return result;
}

bool NavigationMeshBuilder::CalcCellRange(const Box& b, const NavigationHeightfield& height, CellRange& range) const
{
    if (height.GetWidth() == 0 || height.GetDepth() == 0)
    {
        return false;
    }

    if (b.max.x < height.GetWorldBounds().min.x || b.min.x > height.GetWorldBounds().max.x
        || b.max.z < height.GetWorldBounds().min.z || b.min.z > height.GetWorldBounds().max.z)
    {
        return false;
    }

    int32 min_x = static_cast<int32>((std::floor)((b.min.x - height.GetWorldBounds().min.x) / height.GetCellSize()));
    int32 min_z = static_cast<int32>((std::floor)((b.min.z - height.GetWorldBounds().min.z) / height.GetCellSize()));
    int32 max_x = static_cast<int32>((std::floor)((b.max.x - height.GetWorldBounds().min.x) / height.GetCellSize()));
    int32 max_z = static_cast<int32>((std::floor)((b.max.z - height.GetWorldBounds().min.z) / height.GetCellSize()));
    range.min_width_cell = std::clamp(min_x, 0, static_cast<int32>(height.GetWidth() - 1));
    range.min_depth_cell = std::clamp(min_z, 0, static_cast<int32>(height.GetDepth() - 1));
    range.max_width_cell = std::clamp(max_x, 0, static_cast<int32>(height.GetWidth() - 1));
    range.max_depth_cell = std::clamp(max_z, 0, static_cast<int32>(height.GetDepth() - 1));
    return true;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMinX(const std::vector<Vec3>& vertices, float min_x) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.x >= min_x;
        const bool end_inside = end.x >= min_x;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (min_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (min_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMaxX(const std::vector<Vec3>& vertices, float max_x) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.x <= max_x;
        const bool end_inside = end.x <= max_x;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (max_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (max_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMinZ(const std::vector<Vec3>& vertices, float min_z) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.z >= min_z;
        const bool end_inside = end.z >= min_z;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (min_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (min_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMaxZ(const std::vector<Vec3>& vertices, float max_z) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.z <= max_z;
        const bool end_inside = end.z <= max_z;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (max_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (max_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

bool NavigationMeshBuilder::CalcPolygonHeightRange(const std::vector<Vec3>& vertices,
                                                   float& low_height, float& high_height) const
{
    if (vertices.empty())
    {
        return false;
    }
    float low = FLT_MAX;
    float high = -FLT_MAX;
    for (const Vec3& v : vertices)
    {
        if (v.y < low)
        {
            low = v.y;
        }
        if (v.y > high)
        {
            high = v.y;
        }
    }
    low_height = low;
    high_height = high;
    return true;
}

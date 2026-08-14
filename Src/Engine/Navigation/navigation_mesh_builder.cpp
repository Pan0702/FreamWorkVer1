#include "navigation_mesh_builder.h"
#include <algorithm>
#include <map>
#include <queue>
#include <ranges>
#include <cmath>
#include <tuple>

#include "navigation_config.h"
#include "navigation_geometry.h"
#include "navigation_heightfield.h"
#include "navigation_compact_heightfield.h"
#include "../../Debug/debug.h"

namespace
{
    // 方向番号は0:+X、1:+Z、2:-X、3:-Z。接続配列と境界ビットで共通して使う。
    constexpr int32 kDirectionX[4] = {1, 0, -1, 0};
    constexpr int32 kDirectionZ[4] = {0, 1, 0, -1};

    enum class ClipBoundary : uint8
    {
        kMinX,
        kMaxX,
        kMinZ,
        kMaxZ,
    };

    /**
     * @brief 切り抜き境界が使用する座標を頂点から取得する。
     * @param vertex 対象の頂点。
     * @param boundary 使用する軸を表す境界。
     * @return X境界の場合はX座標、Z境界の場合はZ座標。
     */
    float GetClipCoordinate(const Vec3& vertex, ClipBoundary boundary)
    {
        return boundary == ClipBoundary::kMinX || boundary == ClipBoundary::kMaxX
                   ? vertex.x
                   : vertex.z;
    }

    /**
     * @brief 座標が切り抜き境界の内側にあるか判定する。
     * @param coordinate 判定するXまたはZ座標。
     * @param limit 境界の座標。
     * @param boundary 最小側または最大側のどちらを残すかを表す境界。
     * @return 境界の内側または境界上ならtrue。
     */
    bool IsInsideClipBoundary(float coordinate, float limit, ClipBoundary boundary)
    {
        const bool is_minimum = boundary == ClipBoundary::kMinX || boundary == ClipBoundary::kMinZ;
        return is_minimum ? coordinate >= limit : coordinate <= limit;
    }

    /**
     * @brief 多角形をXZ平面上の指定した1境界で切り抜く。
     * @param vertices 切り抜く多角形の頂点列。
     * @param limit 境界の座標。
     * @param boundary 切り抜きに使用する境界。
     * @return 境界の内側に残った多角形の頂点列。
     */
    std::vector<Vec3> ClipPolygonAgainstBoundary(const std::vector<Vec3>& vertices, float limit,
                                                 ClipBoundary boundary)
    {
        // 多角形の各辺を1本ずつ調べ、指定した境界の内側にある部分だけを残す。
        std::vector<Vec3> result;
        if (vertices.empty())
        {
            return result;
        }

        for (uint32 i = 0; i < vertices.size(); ++i)
        {
            const Vec3 start = vertices[i];
            const Vec3 end = vertices[(i + 1) % vertices.size()];
            const float start_coordinate = GetClipCoordinate(start, boundary);
            const float end_coordinate = GetClipCoordinate(end, boundary);
            const bool start_inside = IsInsideClipBoundary(start_coordinate, limit, boundary);
            const bool end_inside = IsInsideClipBoundary(end_coordinate, limit, boundary);

            if (start_inside && end_inside)
            {
                result.push_back(end);
                continue;
            }

            if (!start_inside && !end_inside)
            {
                continue;
            }

            const float t = (limit - start_coordinate) / (end_coordinate - start_coordinate);
            const Vec3 intersection = start + t * (end - start);

            // 外から内へ入る辺では交点と終点、内から外へ出る辺では交点だけを追加する。
            if (start_inside)
            {
                result.push_back(intersection);
            }
            else
            {
                result.push_back(intersection);
                result.push_back(end);
            }
        }
        return result;
    }

    /**
     * @brief 三角形の2辺から法線を計算する。
     * @param triangle 対象の三角形。
     * @return 正規化されていない三角形の法線。
     */
    Vec3 CalcTriangleNormal(const Triangle& triangle)
    {
        return Cross(triangle.b - triangle.a, triangle.c - triangle.a);
    }

    /**
     * @brief 面の傾斜がエージェントの歩行可能角度以内か判定する。
     * @param normal 判定する面の法線。
     * @param config 歩行可能な最大傾斜角を持つ設定。
     * @return 歩行可能な傾斜ならtrue。
     */
    bool IsWalkableNormal(Vec3 normal, const NavigationConfig& config)
    {
        normal.Normalize();
        // ワールドの上方向との内積から傾斜角を判定する。
        const Vec3 kUp(0, 1, 0);
        return Dot(normal, kUp) >= std::cos(config.agent_max_slope_deg * kDegToRad);
    }

    /**
     * @brief 2つのSpan間をエージェントが移動できるか判定する。
     * @param current_floor 現在Spanの床の高さ。
     * @param current_ceiling 現在Spanの天井の高さ。
     * @param neighbor_floor 隣接Spanの床の高さ。
     * @param neighbor_ceiling 隣接Spanの天井の高さ。
     * @param config エージェントの高さと登れる段差を持つ設定。
     * @return 共通の空間と段差の条件を満たすならtrue。
     */
    bool CanTraverseBetweenSpans(float current_floor, float current_ceiling,
                                 float neighbor_floor, float neighbor_ceiling,
                                 const NavigationConfig& config)
    {
        // 2つのSpanで共通して空いている高さと、床同士の段差を両方満たす必要がある。
        const float overlap_floor = (std::max)(current_floor, neighbor_floor);
        const float overlap_ceiling = (std::min)(current_ceiling, neighbor_ceiling);
        const float overlap_height = overlap_ceiling - overlap_floor;
        const float floor_difference = std::abs(neighbor_floor - current_floor);
        return overlap_height >= config.agent_height && floor_difference <= config.agent_max_climb;
    }

    /**
     * @brief Regionごとの歩行可能Span数を集計する。
     * @param heightfield Region分割済みのCompact Heightfield。
     * @param region_count Region IDを添字として確保する配列の要素数。
     * @return Region IDごとの歩行可能Span数。
     */
    std::vector<uint32> CountRegionSpans(const NavigationCompactHeightfield& heightfield, uint32 region_count)
    {
        std::vector<uint32> region_span_counts(region_count, 0);
        for (const NavigationCompactSpan& span : heightfield.GetSpans())
        {
            // Region ID 0は未所属を表すため、領域サイズには数えない。
            if (span.is_walk && span.region_id != 0)
            {
                ++region_span_counts[span.region_id];
            }
        }
        return region_span_counts;
    }

    /**
     * @brief 歩行不可Spanに関係する接続を無効化する。
     * @param heightfield 接続情報を更新するCompact Heightfield。
     */
    void RemoveInvalidConnections(NavigationCompactHeightfield& heightfield)
    {
        // 歩行不可Spanと、歩行不可Spanへ向かう接続をまとめて無効化する。
        const uint32 span_count = static_cast<uint32>(heightfield.GetSpans().size());
        for (uint32 span_index = 0; span_index < span_count; ++span_index)
        {
            NavigationCompactSpan* span = heightfield.GetSpan(span_index);
            for (uint32& neighbor_index : span->connection_indices)
            {
                if (!span->is_walk)
                {
                    neighbor_index = UINT32_MAX;
                    continue;
                }

                if (neighbor_index != UINT32_MAX && !heightfield.GetSpan(neighbor_index)->is_walk)
                {
                    neighbor_index = UINT32_MAX;
                }
            }
        }
    }

    /**
     * @brief XZ平面で点に最も近い線分上の位置を比率で求める。
     * @param point 線分へ投影する点。
     * @param start 線分の始点。
     * @param end 線分の終点。
     * @return 始点を0、終点を1とした線分上の比率。
     */
    float CalcPointToSegmentParameterXZ(const NavigationContourVertex& point,
                                        const NavigationContourVertex& start,
                                        const NavigationContourVertex& end)
    {
        const float segment_x = static_cast<float>(end.x) - static_cast<float>(start.x);
        const float segment_z = static_cast<float>(end.z) - static_cast<float>(start.z);
        const float point_x = static_cast<float>(point.x) - static_cast<float>(start.x);
        const float point_z = static_cast<float>(point.z) - static_cast<float>(start.z);
        const float segment_length_sq = segment_x * segment_x + segment_z * segment_z;

        // 長さ0の線分では始点を最近傍点として扱う。
        if (segment_length_sq <= kEpsilon)
        {
            return 0.0f;
        }
        return std::clamp((point_x * segment_x + point_z * segment_z) / segment_length_sq, 0.0f, 1.0f);
    }
}

Box NavigationMeshBuilder::CalcCellBounds(const NavigationHeightfield& height, uint32 x, uint32 z) const
{
    // 格子座標をワールド座標へ戻し、Heightfield端では最大値を範囲内へ丸める。
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
    // XZの4境界を1面ずつ適用し、三角形のうちCell内に入る多角形を作る。
    std::vector<Vec3> result = {tri.a, tri.b, tri.c};
    result = ClipPolygonAgainstBoundary(result, b.min.x, ClipBoundary::kMinX);
    result = ClipPolygonAgainstBoundary(result, b.max.x, ClipBoundary::kMaxX);
    result = ClipPolygonAgainstBoundary(result, b.min.z, ClipBoundary::kMinZ);
    result = ClipPolygonAgainstBoundary(result, b.max.z, ClipBoundary::kMaxZ);
    return result;
}

bool NavigationMeshBuilder::Build(const std::vector<NavigationGeometry>& geometries, const NavigationConfig& config,
                                  NavigationHeightfield& heightfield)
{
    // 全形状をSpanへ変換した後、重なりの統合と歩行条件のフィルタを順に行う。
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
    if (geometry.indices.empty() || geometry.indices.size() % 3 != 0)
    {
        return false;
    }

    for (uint32 vertex_index : geometry.indices)
    {
        if (vertex_index >= geometry.vertices.size())
        {
            return false;
        }
    }

    bool success_flag = false;

    // 従来の表面Rasterize。
    // Cell端や斜面の高さ情報を維持するため、これは残す。
    // インデックス3個で1三角形を表すため、3個ずつワールド空間へ変換する。
    for (uint32 index = 0; index < geometry.indices.size(); index += 3)
    {
        const Triangle triangle = GetWorldTriangle(geometry, index);
        success_flag |= RasterizeTriangle(triangle, config, height);
    }

    const bool is_closed_geometry = IsClosedGeometry(geometry);

    // 閉じた形状だけ内部をSolid Spanで埋める。
    if (is_closed_geometry)
    {
        success_flag |= RasterizeSolidGeometry(geometry, config, height);
    }
    return success_flag;
}

bool NavigationMeshBuilder::RasterizeTriangle(const Triangle& tri, const NavigationConfig& config,
                                              NavigationHeightfield& height) const
{
    const Vec3 triangle_normal = CalcTriangleNormal(tri);

    // 面積がほぼ0の三角形は高さを作れないので除外する。
    if (triangle_normal.LengthSquared() < kEpsilon)
    {
        return false;
    }

    const bool has_projected_area_xz = std::abs(triangle_normal.y) > kEpsilon;
    Box b = CalcTriangleBounds(tri);
    CellRange range;
    if (!CalcCellRange(b, height, range))
    {
        return false;
    }
    const bool is_walk = IsWalkableNormal(triangle_normal, config);
    bool is_push_back = false;
    // 三角形と重なる各Cellで切り抜き、多角形の上下端を1本のSpanとして登録する。
    for (int32 z = range.min_depth_cell; z <= range.max_depth_cell; ++z)
    {
        for (int32 x = range.min_width_cell; x <= range.max_width_cell; ++x)
        {
            const Box cell_b = CalcCellBounds(height, x, z);
            const std::vector<Vec3> clipped_vertices = ClipTriangleToCell(tri, cell_b);
            if (CalcPolygonAreaXZ(clipped_vertices) <= kEpsilon)
            {
                continue;
            }
            float min_y, max_y;
            CalcPolygonHeightRange(clipped_vertices, min_y, max_y);
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
    // 歩行可能Spanだけを連続配列へ詰め、床高と頭上の空き高さを保持する。
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
                // 上にSpanがなければUINT32_MAXを無限の天井として扱う。
                compact_span.clearance_height = s + 1 < spans->spans.size()
                                                    ? spans->spans[s + 1].min_height - span.max_height
                                                    : UINT32_MAX;
                compact_span.floor_height = span.max_height;

                output.AddSpan(x, z, compact_span);
            }
        }
    }
    // 接続、壁からの半径削り、Region分割の順でCompact Heightfieldを完成させる。
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
    // Region ID 0は未所属なので、実際のRegionは1から割り当てる。
    uint32 next_region_id = 1;
    // 距離場の直交1Cellは2単位。壁から遠いレベルから2ずつ下げて領域を広げる。
    for (int32 min = static_cast<int32>(max_dist); min >= 0; min -= 2)
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
    // 各Spanの4辺を4bitの境界マスクで表し、未追跡の辺から輪郭をたどる。
    std::vector<uint8> boundary_masks = BuildContourBoundaryMasks(heightfield);
    // 設定値はワールド単位なので、比較に使う格子単位へ変換する。
    const float max_error_in_cells = config.max_contour_simplification_error / heightfield.GetCellSize();
    const float max_height_error_in_cells = config.max_contour_height_error / heightfield.GetCellHeight();
    const float max_edge_len_in_cells = config.max_edge_len / heightfield.GetCellSize();
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
                // 4方向のうち境界ビットが残っている辺だけを開始点にする。
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
                            if (SimplifyContour(contour, max_error_in_cells, max_height_error_in_cells,
                                                max_edge_len_in_cells, simplified_contour))
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

bool NavigationMeshBuilder::BuildNavigationMeshData(
    const NavigationCompactHeightfield& heightfield,
    const std::vector<NavigationContour>& contours,
    const NavigationConfig& config,
    NavigationMeshData& mesh_data) const
{
    // ポリゴンは最低3頂点必要。
    if (contours.empty() || config.max_vertex_per_poly < 3)
    {
        return false;
    }

    NavigationMeshData generated_mesh_data;

    for (const NavigationContour& contour : contours)
    {
        std::vector<NavigationContourPolygon> polygons;

        // 輪郭を一度三角形化し、凸形状を保てる範囲で指定頂点数まで結合する。
        if (!BuildContourPolygons(
            contour,
            config.max_vertex_per_poly,
            polygons))
        {
            continue;
        }

        for (const NavigationContourPolygon& polygon : polygons)
        {
            NavigationMeshPolygon mesh_polygon;
            mesh_polygon.region_id = contour.region_id;
            mesh_polygon.vertex_indices.reserve(
                polygon.vertex_indices.size());

            for (uint32 contour_vertex_index : polygon.vertex_indices)
            {
                const uint32 mesh_vertex_index =
                    FindOrAddNavigationMeshVertex(
                        contour.vertices[contour_vertex_index], heightfield, generated_mesh_data);

                mesh_polygon.vertex_indices.push_back(
                    mesh_vertex_index);
            }

            generated_mesh_data.polygons.push_back(mesh_polygon);
        }
    }

    if (generated_mesh_data.polygons.empty())
    {
        return false;
    }

    // 全ポリゴンが揃ってから共有辺を照合し、経路探索用の隣接情報を作る。
    BuildPolygonAdjacency(generated_mesh_data);

    mesh_data.vertices.swap(generated_mesh_data.vertices);
    mesh_data.polygons.swap(generated_mesh_data.polygons);
    return true;
}

float NavigationMeshBuilder::CalcDetailTriangleMaxPenetration(
    const NavigationCompactHeightfield& heightfield,
    const NavigationConfig& config,
    const Vec3& a,
    const Vec3& b,
    const Vec3& c,
    uint32 region_id,
    Vec3& out_position) const
{
    // 4分割した三角格子を検査し、詳細面が元の歩行面へ最も深く入る位置を探す。
    constexpr uint32 kProbeSubdivisionCount = 4;

    // 同じ地表層として参照できる高さ差は、登れる段差に量子化1Cell分を足して許容する。
    const float max_height_diff = config.agent_max_climb + heightfield.GetCellHeight();
    float max_penetration = 0.0f;
    out_position = Vec3(0.0f, 0.0f, 0.0f);

    for (uint32 b_index = 0; b_index <= kProbeSubdivisionCount; ++b_index)
    {
        const uint32 row_vertex_count = kProbeSubdivisionCount - b_index + 1;

        for (uint32 c_index = 0; c_index < row_vertex_count; ++c_index)
        {
            // a、b、cの重みの合計を1にして三角形内部を均等にサンプリングする。
            const float b_weight =
                static_cast<float>(b_index) / static_cast<float>(kProbeSubdivisionCount);
            const float c_weight =
                static_cast<float>(c_index) / static_cast<float>(kProbeSubdivisionCount);
            const float a_weight = 1.0f - b_weight - c_weight;

            const Vec3 probe_position(
                a.x * a_weight + b.x * b_weight + c.x * c_weight,
                a.y * a_weight + b.y * b_weight + c.y * c_weight,
                a.z * a_weight + b.z * b_weight + c.z * c_weight);

            float surface_height = 0.0f;
            if (!TrySampleSurfaceHeight(
                heightfield,
                probe_position.x,
                probe_position.z,
                region_id,
                probe_position.y,
                surface_height,
                max_height_diff))
            {
                continue;
            }

            const float penetration = surface_height - probe_position.y;
            if (penetration <= max_penetration)
            {
                continue;
            }

            max_penetration = penetration;
            out_position = probe_position;
        }
    }

    return max_penetration;
}

void NavigationMeshBuilder::AppendUniformDetailTriangle(const NavigationCompactHeightfield& heightfield,
                                                        const NavigationConfig& config, const Vec3& a, const Vec3& b,
                                                        const Vec3& c, uint32 region_id,
                                                        NavigationDetailMeshData& detail_mesh_data) const
{
    // 0分割では面を作れないため、設定値が0でも最低1分割にする。
    const uint32 subdivision_count = (std::max)(1u, config.detail_subdivision_count);

    const float max_height_diff = config.agent_max_climb + heightfield.GetCellHeight();

    // 三角形を上から段ごとの頂点列として作る。各段は1頂点ずつ短くなる。
    std::vector<std::vector<uint32>> vertex_indices(subdivision_count + 1);

    for (uint32 b_index = 0; b_index <= subdivision_count; ++b_index)
    {
        const uint32 row_vertex_count = subdivision_count - b_index + 1;

        vertex_indices[b_index].reserve(row_vertex_count);

        for (uint32 c_index = 0; c_index < row_vertex_count; ++c_index)
        {
            const float b_weight = static_cast<float>(b_index) / static_cast<float>(subdivision_count);
            const float c_weight = static_cast<float>(c_index) / static_cast<float>(subdivision_count);
            const float a_weight = 1.0f - b_weight - c_weight;

            Vec3 vertex(
                a.x * a_weight + b.x * b_weight + c.x * c_weight,
                a.y * a_weight + b.y * b_weight + c.y * c_weight,
                a.z * a_weight + b.z * b_weight + c.z * c_weight);

            // 共有辺では隣接する三角形も同じ値になるように、
            // 処理順ではなく元の三角形上の補間高さを基準にする。
            const float reference_height = vertex.y;

            float sampled_height = 0.0f;

            if (TrySampleSurfaceHeight(heightfield, vertex.x, vertex.z, region_id, reference_height,
                                       sampled_height, max_height_diff))
            {
                vertex.y = sampled_height;
            }

            const uint32 vertex_index = static_cast<uint32>(detail_mesh_data.vertices.size());
            detail_mesh_data.vertices.push_back(vertex);
            vertex_indices[b_index].push_back(vertex_index);
        }
    }

    // 隣り合う2段の間を、まず1枚、幅が残る場所ではもう1枚の三角形で埋める。
    for (uint32 row = 0; row < subdivision_count; ++row)
    {
        const uint32 next_row_vertex_count = static_cast<uint32>(vertex_indices[row + 1].size());

        for (uint32 column = 0; column < next_row_vertex_count; ++column)
        {
            detail_mesh_data.indices.push_back(vertex_indices[row][column]);
            detail_mesh_data.indices.push_back(vertex_indices[row + 1][column]);
            detail_mesh_data.indices.push_back(vertex_indices[row][column + 1]);

            if (column + 1 >= next_row_vertex_count)
            {
                continue;
            }

            detail_mesh_data.indices.push_back(vertex_indices[row + 1][column]);
            detail_mesh_data.indices.push_back(vertex_indices[row + 1][column + 1]);
            detail_mesh_data.indices.push_back(vertex_indices[row][column + 1]);
        }
    }
}


bool NavigationMeshBuilder::TryGetClosestSpanFloorHeight(const NavigationCompactHeightfield& heightfield, int32 cell_x,
                                                         int32 cell_z, uint32 region_id, float reference_height,
                                                         float& out_height) const
{
    out_height = 0.0f;
    if (region_id == 0 || cell_x < 0 || cell_z < 0 ||
        cell_x >= heightfield.GetWidth() || cell_z >= heightfield.GetDepth())
    {
        return false;
    }
    const NavigationCompactCell* cell = heightfield.GetCell(static_cast<uint32>(cell_x), static_cast<uint32>(cell_z));

    // 同じXZ Cellに複数階があるため、同じRegion内で基準高さに最も近い床を選ぶ。
    bool found_span = false;
    float closest_diff = 0.0f;
    for (uint32 span_offset = 0; span_offset < cell->span_count; ++span_offset)
    {
        const uint32 span_index = cell->first_span_index + span_offset;
        const NavigationCompactSpan* span = heightfield.GetSpan(span_index);

        if (span == nullptr || !span->is_walk || span->region_id != region_id)
        {
            continue;
        }

        const float floor_height = heightfield.GetWorldBounds().min.y + static_cast<float>(span->floor_height)
            * heightfield.GetCellHeight();
        const float diff = std::abs(floor_height - reference_height);

        if (!found_span || diff < closest_diff)
        {
            found_span = true;
            closest_diff = diff;
            out_height = floor_height;
        }
    }
    return found_span;
}

bool NavigationMeshBuilder::BuildNavigationDetailMesh(
    const NavigationCompactHeightfield& heightfield,
    const NavigationMeshData& mesh_data,
    const NavigationConfig& config,
    NavigationDetailMeshData& detail_mesh_data) const
{
    DEBUG_LOG(
        "[Navigation] detail_build_begin_v2: mesh_vertices=%zu, polygons=%zu",
        mesh_data.vertices.size(),
        mesh_data.polygons.size());

    if (mesh_data.vertices.empty() || mesh_data.polygons.empty())
    {
        DEBUG_LOG("[Navigation] detail_build_failed: empty mesh data");
        return false;
    }

    NavigationDetailMeshData generated_detail_mesh_data;
    float max_detail_penetration = 0.0f;
    Vec3 max_detail_penetration_position = {};

    // 各凸ポリゴンを頂点0基準の三角形へ分け、それぞれを均等に細分化する。
    for (const NavigationMeshPolygon& polygon : mesh_data.polygons)
    {
        // Region ID 0は歩行領域に所属しない。3頂点未満も面を作れない。
        if (polygon.region_id == 0 ||
            polygon.vertex_indices.size() < 3)
        {
            continue;
        }

        for (uint32 vertex_index : polygon.vertex_indices)
        {
            if (vertex_index >= mesh_data.vertices.size())
            {
                DEBUG_LOG(
                    "[Navigation] detail_build_failed: vertex index out of range (%u >= %zu)",
                    vertex_index,
                    mesh_data.vertices.size());
                return false;
            }
        }

        const uint32 first_index = polygon.vertex_indices[0];
        const Vec3& first_vertex =
            mesh_data.vertices[first_index];

        for (uint32 index = 1;
             index + 1 < polygon.vertex_indices.size();
             ++index)
        {
            const uint32 second_index =
                polygon.vertex_indices[index];

            const uint32 third_index =
                polygon.vertex_indices[index + 1];

            const Vec3& second_vertex =
                mesh_data.vertices[second_index];

            const Vec3& third_vertex =
                mesh_data.vertices[third_index];

            const size_t first_new_index = generated_detail_mesh_data.indices.size();

            AppendUniformDetailTriangle(heightfield, config, first_vertex,
                                        second_vertex, third_vertex, polygon.region_id,
                                        generated_detail_mesh_data);

            // 追加した詳細三角形が地表へめり込んでいないかを診断用に計測する。
            for (size_t detail_index = first_new_index;
                 detail_index + 2 < generated_detail_mesh_data.indices.size();
                 detail_index += 3)
            {
                const uint32 detail_first_index = generated_detail_mesh_data.indices[detail_index];
                const uint32 detail_second_index = generated_detail_mesh_data.indices[detail_index + 1];
                const uint32 detail_third_index = generated_detail_mesh_data.indices[detail_index + 2];

                Vec3 penetration_position = {};
                const float penetration = CalcDetailTriangleMaxPenetration(
                    heightfield,
                    config,
                    generated_detail_mesh_data.vertices[detail_first_index],
                    generated_detail_mesh_data.vertices[detail_second_index],
                    generated_detail_mesh_data.vertices[detail_third_index],
                    polygon.region_id,
                    penetration_position);

                if (penetration <= max_detail_penetration)
                {
                    continue;
                }

                max_detail_penetration = penetration;
                max_detail_penetration_position = penetration_position;
            }
        }
    }

    if (generated_detail_mesh_data.indices.empty())
    {
        DEBUG_LOG(
            "[Navigation] detail_build_failed: generated detail mesh is empty "
            "(vertices=%zu, indices=%zu)",
            generated_detail_mesh_data.vertices.size(),
            generated_detail_mesh_data.indices.size());
        return false;
    }

    DEBUG_LOG(
        "[Navigation] detail_max_penetration=%.3f, position=(%.3f, %.3f, %.3f)",
        max_detail_penetration,
        max_detail_penetration_position.x,
        max_detail_penetration_position.y,
        max_detail_penetration_position.z);

    detail_mesh_data.vertices.swap(generated_detail_mesh_data.vertices);
    detail_mesh_data.indices.swap(generated_detail_mesh_data.indices);

    return true;
}

bool NavigationMeshBuilder::TrySampleSurfaceHeight(const NavigationCompactHeightfield& heightfield,
                                                   float world_x, float world_z, uint32 region_id,
                                                   float reference_height, float& out_height,
                                                   float max_height_diff) const
{
    out_height = 0.0f;

    if (region_id == 0 || heightfield.GetWidth() == 0 || heightfield.GetDepth() == 0 ||
        heightfield.GetCellSize() <= 0.0f || max_height_diff < 0.0f)
    {
        return false;
    }

    const Box& bounds = heightfield.GetWorldBounds();

    if (world_x < bounds.min.x || world_x > bounds.max.x ||
        world_z < bounds.min.z || world_z > bounds.max.z)
    {
        return false;
    }
    // ワールド座標を格子座標へ変換し、所属CellとCell内の比率へ分ける。
    const float grid_x = (world_x - bounds.min.x) / heightfield.GetCellSize();
    const float grid_z = (world_z - bounds.min.z) / heightfield.GetCellSize();

    const int32 last_cell_x = static_cast<int32>(heightfield.GetWidth()) - 1;
    const int32 last_cell_z = static_cast<int32>(heightfield.GetDepth()) - 1;

    const int32 cell_x = std::clamp(static_cast<int32>(std::floor(grid_x)), 0, last_cell_x);
    const int32 cell_z = std::clamp(static_cast<int32>(std::floor(grid_z)), 0, last_cell_z);

    float layer_height = 0.0f;

    if (!TryGetClosestSpanFloorHeight(heightfield, cell_x, cell_z, region_id, reference_height, layer_height))
    {
        return false;
    }

    if (std::abs(layer_height - reference_height) > max_height_diff)
    {
        return false;
    }

    const float local_x = std::clamp(grid_x - static_cast<float>(cell_x), 0.0f, 1.0f);
    const float local_z = std::clamp(grid_z - static_cast<float>(cell_z), 0.0f, 1.0f);

    // Cellを囲む4隅の高さを求め、X方向、Z方向の順に線形補間する。
    const float height_00 = CalcSurfaceCornerHeight(heightfield, cell_x, cell_z, region_id,
                                                    layer_height, max_height_diff);
    const float height_10 = CalcSurfaceCornerHeight(heightfield, cell_x + 1, cell_z, region_id,
                                                    layer_height, max_height_diff);
    const float height_01 = CalcSurfaceCornerHeight(heightfield, cell_x, cell_z + 1, region_id,
                                                    layer_height, max_height_diff);
    const float height_11 = CalcSurfaceCornerHeight(heightfield, cell_x + 1, cell_z + 1, region_id,
                                                    layer_height, max_height_diff);

    const float height_x_0 = height_00 + (height_10 - height_00) * local_x;
    const float height_x_1 = height_01 + (height_11 - height_01) * local_x;

    out_height = height_x_0 + (height_x_1 - height_x_0) * local_z;

    return true;
}

bool NavigationMeshBuilder::RasterizeSolidGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                                                   NavigationHeightfield& heightfield) const
{
    // 各Cell中央を通る垂直線と閉じたMeshの交点を集め、内部区間をSolid Spanへ変換する。
    std::vector<std::vector<SolidIntersection>> cell_intersections;

    if (!CollectSolidIntersections(geometry, config, heightfield, cell_intersections))
    {
        return false;
    }

    const uint32 width = heightfield.GetWidth();
    const uint32 depth = heightfield.GetDepth();

    bool added_span = false;

    for (uint32 z = 0; z < depth; ++z)
    {
        for (uint32 x = 0; x < width; ++x)
        {
            const size_t cell_index = static_cast<size_t>(z) * width + x;
            std::vector<SolidIntersection>& intersections = cell_intersections[cell_index];

            // Solidの入口と出口が必要なので、交点が2個未満なら内部区間は作れない。
            if (intersections.size() < 2)
            {
                continue;
            }

            if (RasterizeSolidCell(x, z, intersections, heightfield))
            {
                added_span = true;
            }
        }
    }

    return added_span;
}

bool NavigationMeshBuilder::RasterizeSolidCell(uint32 x, uint32 z, std::vector<SolidIntersection>& intersections,
                                               NavigationHeightfield& heightfield) const
{
    std::ranges::sort(intersections,
                      [](const SolidIntersection& left, const SolidIntersection& right)
                      {
                          return left.height < right.height;
                      });
    // 同一面の重複交点をまとめる許容値。極小値とCell高の0.1%の大きい方を使う。
    const float height_tolerance = (std::max)(0.0001f, heightfield.GetCellHeight() * 0.001f);
    int32 solid_depth = 0;
    float solid_start_height = 0.0f;
    bool has_solid_start = false;
    bool added_span = false;
    size_t intersection_index = 0;

    while (intersection_index < intersections.size())
    {
        const float group_min_height = intersections[intersection_index].height;
        float group_max_height = group_min_height;
        int32 group_depth_delta = 0;
        bool group_walkable_top = false;
        size_t next_index = intersection_index;
        while (next_index < intersections.size())
        {
            const float height_difference = std::abs(intersections[next_index].height - group_min_height);
            const bool is_same_height = height_difference <= height_tolerance;

            if (!is_same_height)
            {
                break;
            }

            group_max_height = (std::max)(group_max_height, intersections[next_index].height);
            group_depth_delta += intersections[next_index].depth_delta;
            group_walkable_top |= intersections[next_index].is_walkable_top;

            ++next_index;
        }
        // 面の向きに応じた増減を足し、0以外の区間を閉じたMeshの内部とみなす。
        const int32 previous_depth = solid_depth;
        solid_depth += group_depth_delta;

        // Solidの外側から内側へ入った。
        if (previous_depth == 0 && solid_depth != 0)
        {
            solid_start_height = group_min_height;
            has_solid_start = true;
        }
        // Solidの内側から外側へ出た。
        else if (previous_depth != 0 && solid_depth == 0 && has_solid_start)
        {
            if (group_max_height > solid_start_height + height_tolerance)
            {
                NavigationSpan span;

                if (CreateSpanFromHeightRange(solid_start_height, group_max_height, heightfield,
                                              group_walkable_top, span) && heightfield.AddSpan(x, z, span))
                {
                    added_span = true;
                }
            }

            has_solid_start = false;
        }
        intersection_index = next_index;
    }

    return added_span;
}

bool NavigationMeshBuilder::CollectSolidIntersections(const NavigationGeometry& geometry,
                                                      const NavigationConfig& config,
                                                      const NavigationHeightfield& heightfield,
                                                      std::vector<std::vector<SolidIntersection>>&
                                                      out_cell_intersections) const
{
    out_cell_intersections.clear();

    if (heightfield.GetWidth() == 0 || heightfield.GetDepth() == 0)
    {
        return false;
    }

    const size_t cell_count = static_cast<size_t>(heightfield.GetWidth()) * static_cast<size_t>(heightfield.GetDepth());
    out_cell_intersections.resize(cell_count);
    bool found_intersection = false;
    // インデックス3個で1三角形を表すため、3個ずつ調べる。
    for (uint32 triangle_index = 0; triangle_index < geometry.indices.size(); triangle_index += 3)
    {
        const Triangle triangle =
            GetWorldTriangle(geometry, triangle_index);

        const Vec3 normal = CalcTriangleNormal(triangle);

        if (std::abs(normal.y) <= kEpsilon)
        {
            continue;
        }

        CellRange cell_range;
        if (!CalcCellRange(CalcTriangleBounds(triangle), heightfield, cell_range))
        {
            continue;
        }
        SolidIntersection intersection;
        // 下向き面で内部へ入り、上向き面で外へ出るものとして深さを増減する。
        intersection.depth_delta = normal.y < 0.0f ? 1 : -1;
        intersection.is_walkable_top = normal.y > 0.0f && IsWalkableNormal(normal, config);
        for (int32 z = cell_range.min_depth_cell; z <= cell_range.max_depth_cell; ++z)
        {
            for (int32 x = cell_range.min_width_cell; x <= cell_range.max_width_cell; ++x)
            {
                const Box cell_bounds = CalcCellBounds(heightfield, x, z);

                // Cell中央を通る垂直線を使い、各三角形との交差高さを1点求める。
                const float sample_x = (cell_bounds.min.x + cell_bounds.max.x) * 0.5f;
                const float sample_z = (cell_bounds.min.z + cell_bounds.max.z) * 0.5f;

                if (!TryCalcVerticalIntersectionHeight(triangle, sample_x, sample_z, intersection.height))
                {
                    continue;
                }
                const size_t cell_index = static_cast<size_t>(z) * heightfield.GetWidth() + static_cast<size_t>(x);
                out_cell_intersections[cell_index].push_back(intersection);
                found_intersection = true;
            }
        }
    }

    return found_intersection;
}

bool NavigationMeshBuilder::TryCalcVerticalIntersectionHeight(const Triangle& triangle, float sample_x,
                                                              float sample_z,
                                                              float& out_height) const
{
    const float edge_1_x = triangle.b.x - triangle.a.x;
    const float edge_1_z = triangle.b.z - triangle.a.z;
    const float edge_2_x = triangle.c.x - triangle.a.x;
    const float edge_2_z = triangle.c.z - triangle.a.z;

    const float denominator = edge_1_x * edge_2_z - edge_1_z * edge_2_x;

    // XZへ投影すると面積がない三角形。
    // 壁のような垂直面なので、上下方向の交差判定には使わない。
    if (std::abs(denominator) <= kEpsilon)
    {
        return false;
    }

    const float point_x = sample_x - triangle.a.x;
    const float point_z = sample_z - triangle.a.z;

    const float weight_b = (point_x * edge_2_z - point_z * edge_2_x) / denominator;

    const float weight_c = (edge_1_x * point_z - edge_1_z * point_x) / denominator;

    const float weight_a = 1.0f - weight_b - weight_c;

    // 辺上の丸め誤差で交点を失わないため、重心座標に微小な余裕を持たせる。
    constexpr float kInsideTolerance = 0.0001f;

    if (weight_a < -kInsideTolerance || weight_b < -kInsideTolerance ||
        weight_c < -kInsideTolerance || weight_a > 1.0f + kInsideTolerance ||
        weight_b > 1.0f + kInsideTolerance || weight_c > 1.0f + kInsideTolerance)
    {
        return false;
    }

    out_height = weight_a * triangle.a.y + weight_b * triangle.b.y + weight_c * triangle.c.y;

    return true;
}

bool NavigationMeshBuilder::IsClosedGeometry(const NavigationGeometry& geometry) const
{
    // 同じ位置なのに別頂点番号を持つMeshを接続して判定するための座標許容値。
    constexpr double kWeldTolerance = 0.0001;

    using PositionKey = std::tuple<int64, int64, int64>;
    using EdgeKey = std::pair<uint32, uint32>;

    std::map<PositionKey, uint32> welded_positions;
    std::vector<uint32> welded_vertex_indices(geometry.vertices.size());

    for (uint32 i = 0; i < geometry.vertices.size(); i++)
    {
        const Vec3& position = geometry.vertices[i];
        const PositionKey pos_key(
            static_cast<int64>(std::llround(static_cast<double>(position.x) / kWeldTolerance)),
            static_cast<int64>(std::llround(static_cast<double>(position.y) / kWeldTolerance)),
            static_cast<int64>(std::llround(static_cast<double>(position.z) / kWeldTolerance)));

        const uint32 new_welded_index = static_cast<uint32>(welded_positions.size());
        const auto [iterator, inserted] = welded_positions.insert({pos_key, new_welded_index});
        welded_vertex_indices[i] = iterator->second;
    }

    // 閉じた三角形Meshでは、頂点を溶接した後の各辺が偶数回使われる。
    std::map<EdgeKey, uint32> edge_counts;
    double signed_volume = 0.0;
    const auto add_edge = [&edge_counts](uint32 start, uint32 end)
    {
        if (start == end)
        {
            return false;
        }

        const EdgeKey edge(
            (std::min)(start, end),
            (std::max)(start, end));

        ++edge_counts[edge];
        return true;
    };

    for (uint32 index = 0; index < geometry.indices.size(); index += 3)
    {
        const uint32 vertex_index_0 = geometry.indices[index];
        const uint32 vertex_index_1 = geometry.indices[index + 1];
        const uint32 vertex_index_2 = geometry.indices[index + 2];

        const uint32 welded_index_0 = welded_vertex_indices[vertex_index_0];
        const uint32 welded_index_1 = welded_vertex_indices[vertex_index_1];
        const uint32 welded_index_2 = welded_vertex_indices[vertex_index_2];

        if (welded_index_0 == welded_index_1 ||
            welded_index_1 == welded_index_2 ||
            welded_index_2 == welded_index_0)
        {
            continue;
        }

        if (!add_edge(welded_index_0, welded_index_1) ||
            !add_edge(welded_index_1, welded_index_2) ||
            !add_edge(welded_index_2, welded_index_0))
        {
            return false;
        }

        const Vec3& a = geometry.vertices[vertex_index_0];
        const Vec3& b = geometry.vertices[vertex_index_1];
        const Vec3& c = geometry.vertices[vertex_index_2];

        const double cross_x = static_cast<double>(b.y) * c.z - static_cast<double>(b.z) * c.y;
        const double cross_y = static_cast<double>(b.z) * c.x - static_cast<double>(b.x) * c.z;
        const double cross_z = static_cast<double>(b.x) * c.y - static_cast<double>(b.y) * c.x;

        // 符号付き体積の6倍を加算する。0なら面だけで体積を持たない。
        signed_volume += static_cast<double>(a.x) * cross_x + static_cast<double>(a.y)
            * cross_y + static_cast<double>(a.z) * cross_z;
    }

    if (edge_counts.empty())
    {
        return false;
    }

    // 開いた端があれば、その辺の使用回数は奇数になる。
    for (const auto& count : edge_counts | std::views::values)
    {
        if (count % 2 != 0)
        {
            return false;
        }
    }

    // 面は閉じていても体積が0ならSolidとして扱えない。
    return std::abs(signed_volume) > kEpsilon;
}

float NavigationMeshBuilder::CalcSurfaceCornerHeight(const NavigationCompactHeightfield& heightfield,
                                                     int32 corner_x, int32 corner_z, uint32 region_id
                                                     , float ref_height, float max_height_diff) const
{
    bool found_height = false;
    float corner_height = 0.0f;

    // 1つの格子角を共有する最大4Cellから、同じ地表層の床高を集める。
    for (int32 z_offset = -1; z_offset <= 0; ++z_offset)
    {
        for (int32 x_offset = -1; x_offset <= 0; ++x_offset)
        {
            const int32 cell_x = corner_x + x_offset;
            const int32 cell_z = corner_z + z_offset;

            float floor_height = 0.0f;

            if (!TryGetClosestSpanFloorHeight(
                heightfield,
                cell_x,
                cell_z,
                region_id,
                ref_height,
                floor_height))
            {
                continue;
            }

            const float height_diff =
                std::abs(floor_height - ref_height);

            if (height_diff > max_height_diff)
            {
                continue;
            }

            // 補間面が床へめり込まないよう、候補のうち最も高い床を角の高さにする。
            if (!found_height || floor_height > corner_height)
            {
                corner_height = floor_height;
                found_height = true;
            }
        }
    }

    return found_height ? corner_height : ref_height;
}

float NavigationMeshBuilder::CalcPolygonAreaXZ(const std::vector<Vec3>& vertices) const
{
    // 靴紐公式で符号付き面積の2倍を求める。
    float area_twice = 0.0f;

    for (uint32 i = 0; i < vertices.size(); ++i)
    {
        const uint32 next = (i + 1) % vertices.size();
        area_twice += vertices[i].x * vertices[next].z - vertices[next].x * vertices[i].z;
    }

    // area_twiceは面積の2倍なので、絶対値を半分に戻す。
    return std::abs(area_twice) * 0.5f;
}

void NavigationMeshBuilder::BuildPolygonAdjacency(NavigationMeshData& mesh_data) const
{
    const uint32 polygon_count = static_cast<uint32>(mesh_data.polygons.size());

    for (NavigationMeshPolygon& polygon : mesh_data.polygons)
    {
        const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
        polygon.neighbor_polygon_indices.assign(vertex_count, UINT32_MAX);
    }

    // 2ポリゴンが同じ頂点番号の辺を逆向きに持つ場合、互いを隣接先へ登録する。
    for (uint32 fir_index = 0; fir_index < polygon_count; ++fir_index)
    {
        NavigationMeshPolygon& fir = mesh_data.polygons[fir_index];
        const uint32 fir_vertex_count = static_cast<uint32>(fir.vertex_indices.size());

        for (uint32 sec_index = fir_index + 1; sec_index < polygon_count; ++sec_index)
        {
            NavigationMeshPolygon& sec = mesh_data.polygons[sec_index];
            const uint32 sec_vertex_count = static_cast<uint32>(sec.vertex_indices.size());

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


                    // 1つの辺に3枚以上が接する、2枚が2辺で接する、どちらもNavMeshでは
                    // 正常な形。割り当て済みならこの組を飛ばすだけにして、メッシュ全体は捨てない。
                    if (fir.neighbor_polygon_indices[fir_edge] != UINT32_MAX ||
                        sec.neighbor_polygon_indices[sec_vertex_index] != UINT32_MAX)
                    {
                        continue;
                    }

                    fir.neighbor_polygon_indices[fir_edge] = sec_index;
                    sec.neighbor_polygon_indices[sec_vertex_index] = fir_index;
                }
            }
        }
    }
}

uint32 NavigationMeshBuilder::FindOrAddNavigationMeshVertex(const NavigationContourVertex& contour_vertex,
                                                            const NavigationCompactHeightfield& heightfield,
                                                            NavigationMeshData& mesh_data) const
{
    const Vec3 world_pos = ConvertContourVertexToWorld(contour_vertex, heightfield);

    // 輪郭同士で同じ格子頂点を共有し、隣接判定に使える同一の頂点番号へまとめる。
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
    // 輪郭が持つ格子座標へCell寸法を掛け、Heightfield原点を足してワールド座標へ戻す。
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
    // 凹輪郭を安全に扱うため、最初に必ず三角形へ分割する。
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

    // 共有辺を持つ2枚が凸のままなら、頂点数上限まで繰り返し結合する。
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

    // 共有辺の2頂点は両方に含まれるため、結合後の頂点数から重複分2を引く。
    const uint32 merged_count = fir_count + sec_count - 2;

    if (merged_count > max_vertex_count)
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

    // 同じXZ位置の重複頂点があると長さ0の辺になるため、先に除外する。
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 fir_index = polygon.vertex_indices[i];
        const auto& fir_vertex = contour.vertices[fir_index];
        for (uint32 j = i + 1; j < vertex_count; ++j)
        {
            const uint32 sec_index = polygon.vertex_indices[j];
            const auto& sec_vertex = contour.vertices[sec_index];

            if (fir_vertex.x == sec_vertex.x && fir_vertex.z == sec_vertex.z)
            {
                return false;
            }
        }
    }
    // 反時計回りを前提に、全ての角が左折または直線なら凸形状になる。
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

    // 隣接する輪郭ポリゴンでは、共有辺の頂点番号が逆順に現れる。
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

    // 三角形未満、または外周として必要な反時計回りでない輪郭は処理できない。
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
    // 単純なn角形は必ずn-2枚の三角形へ分割される。
    generated_triangles.reserve(vertex_count - 2);
    while (indices.size() > 3)
    {
        const uint32 remaining_count = static_cast<uint32>(indices.size());

        // 耳の候補のうち対角線が最短のものを選ぶ。最初に見つかった耳を切ると、
        // 1点から細長い三角形が放射状に並ぶ扇になってしまう。
        bool ear_found = false;
        int64 best_diagonal_sq = 0;
        uint32 best_pos = 0;

        for (uint32 pos = 0; pos < remaining_count; ++pos)
        {
            if (!IsContourEar(contour, indices, pos))
            {
                continue;
            }

            const uint32 prev_pos = (pos + remaining_count - 1) % remaining_count;
            const uint32 next_pos = (pos + 1) % remaining_count;

            const NavigationContourVertex& prev_vertex = contour.vertices[indices[prev_pos]];
            const NavigationContourVertex& next_vertex = contour.vertices[indices[next_pos]];

            const int64 diff_x = static_cast<int64>(next_vertex.x) - static_cast<int64>(prev_vertex.x);
            const int64 diff_z = static_cast<int64>(next_vertex.z) - static_cast<int64>(prev_vertex.z);
            const int64 diagonal_sq = diff_x * diff_x + diff_z * diff_z;

            if (!ear_found || diagonal_sq < best_diagonal_sq)
            {
                ear_found = true;
                best_diagonal_sq = diagonal_sq;
                best_pos = pos;
            }
        }

        if (ear_found)
        {
            const uint32 prev_pos = (best_pos + remaining_count - 1) % remaining_count;
            const uint32 next_pos = (best_pos + 1) % remaining_count;

            NavigationContourTriangle triangle;
            triangle.vertex_indices[0] = indices[prev_pos];
            triangle.vertex_indices[1] = indices[best_pos];
            triangle.vertex_indices[2] = indices[next_pos];

            generated_triangles.push_back(triangle);
            indices.erase(indices.begin() + best_pos);
            continue;
        }

        // 共線の頂点しか残っていない場合、面積が0なのでIsContourEarは必ずfalseを返す。
        // 三角形を作らずにその頂点を1つ落として先へ進む。
        bool removed_collinear = false;

        for (uint32 pos = 0; pos < remaining_count; ++pos)
        {
            const uint32 prev_pos = (pos + remaining_count - 1) % remaining_count;
            const uint32 next_pos = (pos + 1) % remaining_count;

            if (CalcTriangleSignedAreaTwiceXZ(contour.vertices[indices[prev_pos]],
                                              contour.vertices[indices[pos]],
                                              contour.vertices[indices[next_pos]]) == 0)
            {
                indices.erase(indices.begin() + pos);
                removed_collinear = true;
                break;
            }
        }

        //切り取れる角がなかったら
        if (!removed_collinear)
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

bool NavigationMeshBuilder::IsContourEar(const NavigationContour& contour,
                                         const std::vector<uint32>& remaining_indices,
                                         uint32 remaining_position) const
{
    const uint32 remaining_count = static_cast<uint32>(remaining_indices.size());

    const uint32 prev_pos = (remaining_position + remaining_count - 1) % remaining_count;
    const uint32 next_pos = (remaining_position + 1) % remaining_count;

    const uint32 prev_index = remaining_indices[prev_pos];
    const uint32 cur_index = remaining_indices[remaining_position];
    const uint32 next_index = remaining_indices[next_pos];

    const auto& prev_vertex = contour.vertices[prev_index];
    const auto& cur_vertex = contour.vertices[cur_index];
    const auto& next_vertex = contour.vertices[next_index];

    // 耳として切り取れるのは、輪郭の内側を向く面積のある角だけ。
    if (CalcTriangleSignedAreaTwiceXZ(prev_vertex, cur_vertex, next_vertex) <= 0)
    {
        return false;
    }
    // 新しくできる対角線が輪郭の他の辺と交差しないことを確認する。
    for (uint32 edge = 0; edge < remaining_count; ++edge)
    {
        const uint32 edge_next_pos = (edge + 1) % remaining_count;
        const uint32 edge_start_index = remaining_indices[edge];
        const uint32 edge_end_index = remaining_indices[edge_next_pos];

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
    // 耳の三角形内に他の輪郭頂点があれば、そこを切り取ることはできない。
    for (uint32 vertex_index : remaining_indices)
    {
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

    // 3辺に対する符号が混在しなければ、点は三角形の内部または辺上にある。
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
        // 3頂点未満は外周にも穴にもならない。
        if (contour.vertices.size() < 3)
        {
            continue;
        }

        // 反時計回りを外周、時計回りを穴として分類する。
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

        // 交差しない橋で外周と穴をつなぎ、三角形化できる1本の輪郭へ変える。
        MergeHoleIntoContour(outer, hole, bridge_outer_index, bridge_hole_index);
    }
    contours.swap(outer_contours);
    return true;
}

bool NavigationMeshBuilder::FindContainingOuterContour(const NavigationContour& hole,
                                                       const std::vector<NavigationContour>& contours,
                                                       uint32& outer_index) const
{
    // 同じRegionの外周が1つしかなければ、それが親で確定。
    // 幾何判定は、単純化で穴の頂点が外周のわずかに外へ出ただけで失敗する。
    uint32 same_region_count = 0;
    uint32 same_region_index = 0;
    for (uint32 i = 0; i < contours.size(); ++i)
    {
        if (contours[i].region_id != hole.region_id ||
            CalcContourSignedAreaTwice(contours[i]) <= 0)
        {
            continue;
        }
        ++same_region_count;
        same_region_index = i;
    }
    if (same_region_count == 1)
    {
        outer_index = same_region_index;
        return true;
    }
    bool found = false;
    int64 best_area_twice = 0;
    for (uint32 i = 0; i < contours.size(); ++i)
    {
        const NavigationContour& candidate = contours[i];
        if (candidate.region_id != hole.region_id)
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

void NavigationMeshBuilder::MergeHoleIntoContour(NavigationContour& outer, const NavigationContour& hole,
                                                 uint32 outer_index, uint32 hole_index) const
{
    const uint32 outer_count = static_cast<uint32>(outer.vertices.size());
    const uint32 hole_count = static_cast<uint32>(hole.vertices.size());
    std::vector<NavigationContourVertex> merge_vertices;

    // 外周から橋を渡って穴を1周し、同じ橋を戻って外周の続きを連結する。
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
}

bool NavigationMeshBuilder::FindHoleBridge(const NavigationContour& outer, const NavigationContour& hole_contour,
                                           uint32& out_outer_index, uint32& out_hole_index) const
{
    bool found = false;
    double best_dis_sq = 0.0;
    // 外周と穴の全頂点対から、輪郭と交差しない最短の橋を選ぶ。
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
    const auto& outer_vertex = outer.vertices[outer_index];
    const auto& hole_vertex = hole_contour.vertices[hole_index];

    // 橋の両端に接する辺は交差扱いせず、それ以外の外周辺と穴辺を調べる。
    const auto intersects_contour = [this, &outer_vertex, &hole_vertex](const NavigationContour& contour,
                                                                        uint32 bridge_index)
    {
        for (uint32 i = 0; i < contour.vertices.size(); ++i)
        {
            const uint32 next_index = (i + 1) % static_cast<uint32>(contour.vertices.size());
            if (i == bridge_index || next_index == bridge_index)
            {
                continue;
            }
            if (DoSegmentsIntersectXZ(outer_vertex, hole_vertex,
                                      contour.vertices[i], contour.vertices[next_index]))
            {
                return true;
            }
        }
        return false;
    };

    return !intersects_contour(outer, outer_index) &&
        !intersects_contour(hole_contour, hole_index);
}

bool NavigationMeshBuilder::DoSegmentsIntersectXZ(const NavigationContourVertex& a,
                                                  const NavigationContourVertex& b,
                                                  const NavigationContourVertex& c,
                                                  const NavigationContourVertex& d) const
{
    const int64 abc = CalcTriangleSignedAreaTwiceXZ(a, b, c);
    const int64 abd = CalcTriangleSignedAreaTwiceXZ(a, b, d);
    const int64 cda = CalcTriangleSignedAreaTwiceXZ(c, d, a);
    const int64 cdb = CalcTriangleSignedAreaTwiceXZ(c, d, b);
    // 互いの端点が相手線分の両側にあれば交差する。共線時は線分上判定へ進む。
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
    bool inside = false;
    const auto& vertices = contour.vertices;
    // 点から+X方向へ伸ばした半直線と輪郭辺の交差回数が奇数なら内部にある。
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
    int64 area_twice = 0;
    const auto& vertices = contour.vertices;
    // 靴紐公式。正なら反時計回り、負なら時計回りになる。
    for (uint32 i = 0; i < vertex_count; ++i)
    {
        const uint32 next_index = (i + 1) % vertex_count;
        area_twice += static_cast<int64>(vertices[i].x) * static_cast<int64>(vertices[next_index].z)
            - static_cast<int64>(vertices[next_index].x) * static_cast<int64>(vertices[i].z);
    }
    return area_twice;
}

bool NavigationMeshBuilder::SimplifyContour(const NavigationContour& raw_contour, float max_error_in_cells,
                                            float max_height_error_in_cells, float max_edge_len,
                                            NavigationContour& simplified_contour) const
{
    uint32 vertex_count = static_cast<uint32>(raw_contour.vertices.size());
    std::vector<uint8> keep_flags(vertex_count, 0);
    uint32 kept_count = 0;
    // 3頂点未満は単純化しても面にならない。
    if (vertex_count < 3)
    {
        return false;
    }
    simplified_contour.vertices.clear();
    simplified_contour.region_id = raw_contour.region_id;
    // 隣接Regionが切り替わる頂点はポータル形状に必要なので、最初から残す。
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
    // 境界変化が少ない閉輪郭では、XZで離れた両端を最初の2点として選ぶ。
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
    // 2点だけでは面を作れないため、その線分から最も遠い頂点を3点目にする。
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
    // 長すぎる外周辺は中央の生頂点を戻し、詳細Meshが地形へ追従できる長さに分割する。
    if (max_edge_len > 0.0f)
    {
        const float max_edge_len_in_cells_sq = max_edge_len * max_edge_len;
        bool split_vertex = true;

        while (split_vertex)
        {
            split_vertex = false;

            std::vector<uint32> kept_indices;
            for (uint32 i = 0; i < keep_flags.size(); ++i)
            {
                if (keep_flags[i] == 1)
                {
                    kept_indices.push_back(i);
                }
            }

            if (kept_indices.size() < 2)
            {
                break;
            }

            for (uint32 i = 0; i < kept_indices.size(); ++i)
            {
                const uint32 start_index = kept_indices[i];
                const uint32 end_index = kept_indices[(i + 1) % kept_indices.size()];

                const float diff_x = static_cast<float>(raw_contour.vertices[end_index].x) -
                    static_cast<float>(raw_contour.vertices[start_index].x);
                const float diff_z = static_cast<float>(raw_contour.vertices[end_index].z) -
                    static_cast<float>(raw_contour.vertices[start_index].z);

                if (diff_x * diff_x + diff_z * diff_z <= max_edge_len_in_cells_sq)
                {
                    continue;
                }

                // 生の輪郭上で start から end までに何頂点あるか
                const uint32 raw_count = (end_index > start_index)
                                             ? (end_index - start_index)
                                             : (end_index + vertex_count - start_index);
                if (raw_count <= 1)
                {
                    continue;
                }

                // 区間の中央を選ぶことで、繰り返し分割したときに辺長を均等にする。
                const uint32 middle_index = (start_index + raw_count / 2) % vertex_count;
                if (keep_flags[middle_index] == 1)
                {
                    continue;
                }
                const uint32 probe_index = (start_index + 1) % vertex_count;
                if (raw_contour.vertices[probe_index].neighbor_region_id != 0)
                {
                    continue;
                }

                keep_flags[middle_index] = 1;
                ++kept_count;
                split_vertex = true;
                break;
            }
        }
    }
    // 距離誤差と高さ誤差を各許容値で正規化し、大きい側を分割判定に使う。
    const float max_error_in_cells_sq = max_error_in_cells * max_error_in_cells;
    const float max_height_error_in_cells_sq = max_height_error_in_cells * max_height_error_in_cells;
    const float safe_max_error_sq = (std::max)(max_error_in_cells_sq, kEpsilon);
    const float safe_max_height_error_sq = (std::max)(max_height_error_in_cells_sq, kEpsilon);
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
        float max_error_ratio = 0.0f;
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
                const float height_error = CalcPointToSegmentHeightError(raw_contour.vertices[raw_index],
                                                                         raw_contour.vertices[start_index],
                                                                         raw_contour.vertices[end_index]);
                const float height_error_sq = height_error * height_error;
                const float error_ratio = (std::max)(sq_dis / safe_max_error_sq,
                                                     height_error_sq / safe_max_height_error_sq);
                // 同点のときは格子座標が小さい方を選ぶ。走査順で決めると、
                // 同じポータルを逆向きにたどる隣のRegionと違う頂点を選んでしまう。
                bool better = error_ratio > max_error_ratio;
                if (!better && error_ratio == max_error_ratio && max_far_index >= 0)
                {
                    const auto& cand = raw_contour.vertices[raw_index];
                    const auto& cur = raw_contour.vertices[max_far_index];
                    better = (cand.x < cur.x) || (cand.x == cur.x && cand.z < cur.z);
                }
                if (better)
                {
                    max_error_ratio = error_ratio;
                    max_far_index = static_cast<int32>(raw_index);
                }
                raw_index = (raw_index + 1) % vertex_count;
            }
        }


        // 正規化誤差が1を超えたときだけ、最も誤差の大きい生頂点を戻す。
        if (max_error_ratio > 1.0f && max_far_index >= 0)
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
    const float segment_x = static_cast<float>(end.x) - static_cast<float>(start.x);
    const float segment_z = static_cast<float>(end.z) - static_cast<float>(start.z);
    const float t = CalcPointToSegmentParameterXZ(point, start, end);
    const float closest_x = static_cast<float>(start.x) + segment_x * t;
    const float closest_z = static_cast<float>(start.z) + segment_z * t;
    const float distance_x = closest_x - static_cast<float>(point.x);
    const float distance_z = closest_z - static_cast<float>(point.z);
    return distance_x * distance_x + distance_z * distance_z;
}

float NavigationMeshBuilder::CalcPointToSegmentHeightError(const NavigationContourVertex& point,
                                                           const NavigationContourVertex& start,
                                                           const NavigationContourVertex& end) const
{
    const float t = CalcPointToSegmentParameterXZ(point, start, end);
    const float interpolated_height = static_cast<float>(start.height) +
        (static_cast<float>(end.height) - static_cast<float>(start.height)) * t;
    return std::abs(static_cast<float>(point.height) - interpolated_height);
}

uint32 NavigationMeshBuilder::CalcContourCornerHeight(const NavigationCompactHeightfield& heightfield,
                                                      uint32 span_index,
                                                      uint32 direction) const
{
    const auto* span = heightfield.GetSpan(span_index);
    uint32 max_floor_height = span->floor_height;
    // 4方向の1つ前は、負数を避けるため3を足して4で剰余を取る。
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

    // 現在、進行方向、直前方向、斜めの最大4Spanが同じ格子角を共有する。
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
    // 境界なら角を記録して右へ回り、境界でなければ隣Spanへ移って左へ回る。
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
            // bit 1、2、4、8は順に+X、+Z、-X、-Z側の辺を表す。
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
            // 境界に沿い続けるため右へ90度回る。
            direction = (direction + 1) % 4;
        }
        else
        {
            span_index = neighbor_index;
            x = static_cast<uint32>(static_cast<int32>(x) + kDirectionX[direction]);
            z = static_cast<uint32>(static_cast<int32>(z) + kDirectionZ[direction]);
            // 隣へ進めた場合は左へ90度回り、次の境界候補を調べる。
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
        // 接続なし、歩行不可、別Regionのいずれかなら、その方向の1bitを境界として立てる。
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
    const std::vector<uint32> region_span_counts = CountRegionSpans(heightfield, next_id);

    // 小Regionごとに、自分以上のSpan数を持つ隣接Regionを結合先として記録する。
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
            // 同じ大きさなら小さいRegion IDへ寄せ、互いを結合先にする循環を防ぐ。
            if (region_span_counts[neighbor_span->region_id] == region_span_counts[span->region_id]
                && neighbor_span->region_id > span->region_id)
            {
                continue;
            }
            merge_targets[span->region_id] = neighbor_span->region_id;
        }
    }
    // 候補収集が終わってから一括変更し、走査途中のRegion ID変化を判定へ混ぜない。
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

    const std::vector<uint32> region_span_counts = CountRegionSpans(heightfield, next_region_id);

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

    RemoveInvalidConnections(heightfield);
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
    // 既存Regionを種にして、現在の距離レベル以上にある未所属Spanへ番号を広げる。
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
        // 既存Regionが届かなかったSpanを新しい種にし、同じ距離レベル内を塗りつぶす。
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
                if (!neighbor_span->is_walk || neighbor_span->region_id != 0 || neighbor_span->dis_to_wall <
                    min_dist)
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

void NavigationMeshBuilder::ErodeWalkableArea(NavigationCompactHeightfield& heightfield,
                                              const NavigationConfig& config)
{
    const uint32 span_count =
        static_cast<uint32>(heightfield.GetSpans().size());

    // 壁際のSpanを距離0として登録
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        // 0xffffは距離がまだ伝播していないことを表す、十分大きな初期値。
        constexpr uint32 kUnreachedDistance = 0xffffu;
        auto* span = heightfield.GetSpan(span_index);
        span->dis_to_wall = kUnreachedDistance;

        for (uint32 connection_index : span->connection_indices)
        {
            if (connection_index == UINT32_MAX)
            {
                span->dis_to_wall = 0;
                break;
            }
        }
    }

    const auto accumulate = [&heightfield](uint32 span_index, uint32 axis_direction)
    {
        NavigationCompactSpan* span = heightfield.GetSpan(span_index);

        const uint32 neighbor_index = span->connection_indices[axis_direction];
        if (neighbor_index == UINT32_MAX)
        {
            return;
        }

        const NavigationCompactSpan* neighbor_span = heightfield.GetSpan(neighbor_index);
        // 距離場は直交移動を2、斜め移動を3とする整数近似で計算する。
        span->dis_to_wall = (std::min)(neighbor_span->dis_to_wall + 2, span->dis_to_wall);

        // 斜め: 隣から更に1方向進んだ先
        const uint32 diagonal_direction = (axis_direction + 3) % 4;
        const uint32 diagonal_index = neighbor_span->connection_indices[diagonal_direction];
        if (diagonal_index == UINT32_MAX)
        {
            return;
        }

        const NavigationCompactSpan* diagonal_span = heightfield.GetSpan(diagonal_index);
        span->dis_to_wall = (std::min)(diagonal_span->dis_to_wall + 3, span->dis_to_wall);
    };

    // 1パス目: z昇順・x昇順。すでに確定している -x(2) と -z(3) 側から取り込む。
    for (uint32 z = 0; z < heightfield.GetDepth(); ++z)
    {
        for (uint32 x = 0; x < heightfield.GetWidth(); ++x)
        {
            const NavigationCompactCell* cell = heightfield.GetCell(x, z);
            for (uint32 i = 0; i < cell->span_count; ++i)
            {
                const uint32 span_index = cell->first_span_index + i;
                accumulate(span_index, 2);
                accumulate(span_index, 3);
            }
        }
    }

    // 2パス目: z降順・x降順。+x(0) と +z(1) 側から取り込む。
    for (int32 z = static_cast<int32>(heightfield.GetDepth()) - 1; z >= 0; --z)
    {
        for (int32 x = static_cast<int32>(heightfield.GetWidth()) - 1; x >= 0; --x)
        {
            const NavigationCompactCell* cell = heightfield.GetCell(x, z);
            for (uint32 i = 0; i < cell->span_count; ++i)
            {
                const uint32 span_index = cell->first_span_index + i;
                accumulate(span_index, 0);
                accumulate(span_index, 1);
            }
        }
    }

    // 距離場の直交1Cellは2単位なので、Cell数へ変換した半径も2倍する。
    const uint32 radius_in_cells = static_cast<uint32>(std::ceil(config.agent_radius / heightfield.GetCellSize()));
    const uint32 radius_in_dis = radius_in_cells * 2;
    // 半径内のSpanを歩行不可にする
    for (uint32 span_index = 0; span_index < span_count; ++span_index)
    {
        auto* span = heightfield.GetSpan(span_index);

        if (span->dis_to_wall < radius_in_dis)
        {
            span->is_walk = false;
        }
    }

    RemoveInvalidConnections(heightfield);
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
                // UINT32_MAXの空き高さは、上に天井がないものとしてFLT_MAXへ変換する。
                const float current_ceiling_height = span->clearance_height == UINT32_MAX
                                                         ? FLT_MAX
                                                         : span->clearance_height * heightfield.GetCellHeight() +
                                                         current_floor;
                // 各SpanからXZの4近傍へ、段差と共通空間を満たすSpanを1本ずつ接続する。
                for (int connect = 0; connect < 4; ++connect)
                {
                    const int32 neighbor_x = x + kDirectionX[connect];
                    const int32 neighbor_z = z + kDirectionZ[connect];
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

                        if (CanTraverseBetweenSpans(current_floor, current_ceiling_height,
                                                    neighbor_floor, neighbor_ceiling_height, config))
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
    // 4方向のどこか1方向でも到達できる床がなければ、崖際として歩行不可にする。
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
                for (uint32 direction = 0; direction < 4; ++direction)
                {
                    bool has_reachable_neighbor = false;
                    const int32 neighbor_x = x + kDirectionX[direction];
                    const int32 neighbor_z = z + kDirectionZ[direction];
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
                        const float neighbor_floor = neighbor_cell->spans[n].max_height * heightfield.
                            GetCellHeight();
                        const float neighbor_ceiling_height = (n + 1 < neighbor_cell->spans.size())
                                                                  ? neighbor_cell->spans[n + 1].min_height *
                                                                  heightfield
                                                                  .GetCellHeight()
                                                                  : FLT_MAX;

                        if (CanTraverseBetweenSpans(current_floor, current_ceiling_height,
                                                    neighbor_floor, neighbor_ceiling_height, config))
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

void NavigationMeshBuilder::FilterLowCeilingSpans(NavigationHeightfield& heightfield,
                                                  const NavigationConfig& config)
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
                // 現在Spanの上面から次Spanの下面までを頭上の空き高さとして調べる。
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
    // 上下が逆転した高さ範囲はSpanとして扱えない。
    if (min_y > max_y)
    {
        return false;
    }
    // 高さ方向のCell寸法がなければ格子座標へ変換できない。
    if (height.GetCellHeight() <= 0.0f)
    {
        return false;
    }
    // HeightfieldのY範囲と全く重ならない面は登録しない。
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

    // 形状を欠かさないよう、下面は切り捨て、上面は切り上げてSpanを外側へ広げる。
    span.min_height = static_cast<uint32>(std::floor(relative_min));
    span.max_height = static_cast<uint32>(std::ceil(relative_max));

    span.is_walk = is_walk;
    return true;
}

Triangle NavigationMeshBuilder::GetWorldTriangle(const NavigationGeometry& geometry, uint32 begin) const
{
    // begin、begin+1、begin+2の3インデックスを1三角形としてワールド空間へ変換する。
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
    // 3頂点の各軸最小値と最大値から三角形のAABBを作る。
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

    if (b.max.x < height.GetWorldBounds().min.x || b.min.x >
        height.GetWorldBounds().max.x
        || b.max.z < height.GetWorldBounds().min.z || b.min.z >
        height.GetWorldBounds().max.z
    )
    {
        return false;
    }
    const Box& world_bounds = height.GetWorldBounds();
    const float cell_size = height.GetCellSize();

    const float relative_min_x = (b.min.x - world_bounds.min.x) / cell_size;
    const float relative_max_x = (b.max.x - world_bounds.min.x) / cell_size;
    const float relative_min_z = (b.min.z - world_bounds.min.z) / cell_size;
    const float relative_max_z = (b.max.z - world_bounds.min.z) / cell_size;

    // 最大端はceil後の境界Cellを含めないため1を引き、両端を包含するCell範囲にする。
    int32 min_x = static_cast<int32>(std::floor(relative_min_x));
    int32 max_x = static_cast<int32>(std::ceil(relative_max_x)) - 1;
    int32 min_z = static_cast<int32>(std::floor(relative_min_z));
    int32 max_z = static_cast<int32>(std::ceil(relative_max_z)) - 1;

    const int32 last_x =
        static_cast<int32>(height.GetWidth()) - 1;
    const int32 last_z =
        static_cast<int32>(height.GetDepth()) - 1;

    range.min_width_cell = std::clamp(min_x, 0, last_x);
    range.max_width_cell = std::clamp(max_x, 0, last_x);
    range.min_depth_cell = std::clamp(min_z, 0, last_z);
    range.max_depth_cell = std::clamp(max_z, 0, last_z);

    return range.min_width_cell <= range.max_width_cell &&
        range.min_depth_cell <= range.max_depth_cell;
}

void NavigationMeshBuilder::CalcPolygonHeightRange(const std::vector<Vec3>& vertices,
                                                   float& low_height, float& high_height) const
{
    // Cellで切り抜いた多角形の全頂点から、Spanへ変換するY範囲を求める。
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
}

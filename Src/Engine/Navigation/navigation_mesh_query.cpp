#include "navigation_mesh_query.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <list>

bool NavigationMeshQuery::FindNearestPolygon(const NavigationMeshData& mesh_data, const Vec3& position,
                                             uint32& out_polygon_index) const
{
    out_polygon_index = UINT32_MAX;
    float best_dis_sq = FLT_MAX;


    for (uint32 polygon_index = 0; polygon_index < mesh_data.polygons.size(); ++polygon_index)
    {
        const auto& polygon = mesh_data.polygons[polygon_index];
        if (polygon.vertex_indices.size() < 3)
        {
            continue;
        }
        bool valid_polygon = true;
        for (uint32 vertex_index : polygon.vertex_indices)
        {
            if (vertex_index >= mesh_data.vertices.size())
            {
                valid_polygon = false;
                break;
            }
        }

        if (!valid_polygon)
        {
            continue;
        }
        const Vec3& a = mesh_data.vertices[polygon.vertex_indices[0]];

        for (uint32 i = 1; i + 1 < polygon.vertex_indices.size(); ++i)
        {
            const Vec3& b = mesh_data.vertices[polygon.vertex_indices[i]];
            const Vec3& c = mesh_data.vertices[polygon.vertex_indices[i + 1]];
            const Vec3 closest_point = CalcClosestPointOnTriangle(position, a, b, c);

            const float dis_sq = (closest_point - position).LengthSquared();
            if (dis_sq < best_dis_sq)
            {
                best_dis_sq = dis_sq;
                out_polygon_index = polygon_index;
            }
        }
    }
    return out_polygon_index != UINT32_MAX;
}

bool NavigationMeshQuery::FindPolygonPath(const NavigationMeshData& mesh_data, uint32 start_poly, uint32 goal_poly,
                                          const Vec3& start_position, const Vec3& goal_position,
                                          std::vector<uint32>& out_poly) const
{
    out_poly.clear();
    if (start_poly >= mesh_data.polygons.size() || goal_poly >= mesh_data.polygons.size())
    {
        return false;
    }

    if (start_poly == goal_poly)
    {
        out_poly.push_back(start_poly);
        return true;
    }

    std::vector<Node> nodes(mesh_data.polygons.size());
    std::vector<uint32> open;
    bool find_path = false;

    Node& start_node = nodes[start_poly];
    start_node.position = start_position;
    start_node.g_cost = 0.0f;
    start_node.h_cost = (goal_position - start_position).Length();
    start_node.f_cost = start_node.h_cost;
    start_node.state = State::kOpen;

    open.push_back(start_poly);

    while (!open.empty())
    {
        uint32 cur_index = open.front();
        int open_index = 0;
        for (int i = 1; i < open.size(); ++i)
        {
            if (nodes[open[i]].f_cost < nodes[cur_index].f_cost)
            {
                cur_index = open[i];
                open_index = i;
            }
        }
        nodes[cur_index].state = State::kClosed;
        open.erase(open.begin() + open_index);

        if (cur_index == goal_poly)
        {
            out_poly.push_back(goal_poly);
            find_path = true;
            break;
        }

        for (uint32 neighbor_index : mesh_data.polygons[cur_index].neighbor_polygon_indices)
        {
            if (neighbor_index == UINT32_MAX || neighbor_index >= mesh_data.polygons.size())
            {
                continue;
            }
            if (nodes[neighbor_index].state == State::kClosed)
            {
                continue;
            }

            Vec3 edge_a;
            Vec3 edge_b;
            if (!TryGetSharedEdge(mesh_data, cur_index, neighbor_index, edge_a, edge_b))
            {
                continue;
            }

            Vec3 neighbor_position = CalcBestPortalPoint(
                nodes[cur_index].position, goal_position, edge_a, edge_b);
            if (neighbor_index == goal_poly)
            {
                neighbor_position = goal_position;
            }

            const float g_cost = nodes[cur_index].g_cost +
                (neighbor_position - nodes[cur_index].position).Length();
            if (g_cost < nodes[neighbor_index].g_cost || nodes[neighbor_index].state == State::kUnvisited)
            {
                nodes[neighbor_index].parent_index = cur_index;
                nodes[neighbor_index].position = neighbor_position;
                nodes[neighbor_index].g_cost = g_cost;
                nodes[neighbor_index].h_cost = (goal_position - neighbor_position).Length();
                nodes[neighbor_index].f_cost = nodes[neighbor_index].g_cost + nodes[neighbor_index].h_cost;
                if (nodes[neighbor_index].state == State::kUnvisited)
                {
                    nodes[neighbor_index].state = State::kOpen;
                    open.push_back(neighbor_index);
                }
            }
        }
    }


    if (find_path)
    {
        uint32 parent = nodes[out_poly[0]].parent_index;
        while (parent != UINT32_MAX)
        {
            out_poly.push_back(parent);
            parent = nodes[parent].parent_index;
        }

        std::ranges::reverse(out_poly);
        return true;
    }
    else
    {
        return false;
    }
}

bool NavigationMeshQuery::FindStraightPath(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                                           const Vec3& start_position, const Vec3& goal_pos,
                                           std::vector<Vec3>& out_path) const
{
    out_path.clear();

    std::vector<Portal> portals;

    if (!BuildPortals(mesh_data, polygon_path, start_position, goal_pos, portals))
    {
        return false;
    }

    if (portals.empty())
    {
        return false;
    }

    Vec3 apex = portals[0].left;
    Vec3 left = portals[0].left;
    Vec3 right = portals[0].right;

    uint32 apex_index = 0;
    uint32 left_index = 0;
    uint32 right_index = 0;

    out_path.push_back(apex);

    uint32 portal_index = 1;

    while (portal_index < portals.size())
    {
        const Vec3 new_left = portals[portal_index].left;
        const Vec3 new_right =portals[portal_index].right;

        // 右側を狭められるか確認
        if (CalcSignedAreaXZ(apex, right, new_right) >= 0.0f)
        {
            if (IsSamePointXZ(apex, right) ||
                CalcSignedAreaXZ(apex, left, new_right) < 0.0f)
            {
                right = new_right;
                right_index = portal_index;
            }
            else
            {
                // 新しい右端が左側を越えたので、
                // 現在の左端を経路の曲がり角にする
                if (!IsSamePointXZ(out_path.back(), left))
                {
                    out_path.push_back(left);
                }

                apex = left;
                apex_index = left_index;

                left = apex;
                right = apex;

                left_index = apex_index;
                right_index = apex_index;

                portal_index = apex_index + 1;
                continue;
            }
        }

        // 左側を狭められるか確認
        if (CalcSignedAreaXZ(apex, left, new_left) <= 0.0f)
        {
            if (IsSamePointXZ(apex, left) ||
                CalcSignedAreaXZ(apex, right, new_left) > 0.0f)
            {
                left = new_left;
                left_index = portal_index;
            }
            else
            {
                // 新しい左端が右側を越えたので、
                // 現在の右端を経路の曲がり角にする
                if (!IsSamePointXZ(out_path.back(), right))
                {
                    out_path.push_back(right);
                }

                apex = right;
                apex_index = right_index;

                left = apex;
                right = apex;

                left_index = apex_index;
                right_index = apex_index;

                portal_index = apex_index + 1;
                continue;
            }
        }

        ++portal_index;
    }

    if (out_path.empty() ||
        !IsSamePointXZ(out_path.back(), goal_pos))
    {
        out_path.push_back(goal_pos);
    }

    return true;
}

bool NavigationMeshQuery::FindNearestPolygon(const NavigationMeshData& mesh_data, const Vec3& position,
                                             uint32& out_polygon_index, Vec3& out_closest_point) const
{
    out_polygon_index = UINT32_MAX;
    out_closest_point = Vec3();
    float best_dis_sq = FLT_MAX;

    for (uint32 polygon_index = 0; polygon_index < mesh_data.polygons.size();
         ++polygon_index)
    {
        const auto& polygon = mesh_data.polygons[polygon_index];

        if (polygon.vertex_indices.size() < 3)
        {
            continue;
        }

        bool valid_polygon = true;

        for (uint32 vertex_index : polygon.vertex_indices)
        {
            if (vertex_index >= mesh_data.vertices.size())
            {
                valid_polygon = false;
                break;
            }
        }

        if (!valid_polygon)
        {
            continue;
        }

        const Vec3& a = mesh_data.vertices[polygon.vertex_indices[0]];

        for (uint32 i = 1; i + 1 < polygon.vertex_indices.size(); ++i)
        {
            const Vec3& b = mesh_data.vertices[polygon.vertex_indices[i]];
            const Vec3& c = mesh_data.vertices[polygon.vertex_indices[i + 1]];
            const Vec3 closest_point = CalcClosestPointOnTriangle(position, a, b, c);

            const float dist_sq = (closest_point - position).LengthSquared();

            if (dist_sq >= best_dis_sq)
            {
                continue;
            }

            best_dis_sq = dist_sq;
            out_polygon_index = polygon_index;
            out_closest_point = closest_point;
        }
    }

    return out_polygon_index != UINT32_MAX;
}

bool NavigationMeshQuery::FindPath(const NavigationMeshData& mesh_data, const Vec3& start_pos,
                                   const Vec3& goal_pos, std::vector<Vec3>& out_path,
                                   std::vector<uint32>* out_polygon_path) const
{
    out_path.clear();
    if (out_polygon_path != nullptr)
    {
        out_polygon_path->clear();
    }

    uint32 start_polygon_index = UINT32_MAX;
    uint32 goal_polygon_index = UINT32_MAX;

    Vec3 nearest_start_position;
    Vec3 nearest_goal_position;

    if (!FindNearestPolygon(mesh_data, start_pos,
                            start_polygon_index, nearest_start_position))
    {
        return false;
    }

    if (!FindNearestPolygon(mesh_data, goal_pos, goal_polygon_index, nearest_goal_position))
    {
        return false;
    }

    if (TraceSegmentAcrossNavMesh(mesh_data, start_polygon_index, goal_polygon_index,
                                  nearest_start_position, nearest_goal_position,
                                  out_path, out_polygon_path))
    {
        return true;
    }

    std::vector<uint32> polygon_path;

    if (!FindPolygonPath(mesh_data, start_polygon_index, goal_polygon_index,
                         nearest_start_position, nearest_goal_position, polygon_path))
    {
        return false;
    }

    std::vector<Vec3> funnel_path;
    if (!FindStraightPath(mesh_data, polygon_path,
                          nearest_start_position, nearest_goal_position, funnel_path))
    {
        out_path.clear();
        return false;
    }

    std::vector<Vec3> optimized_path;
    std::vector<uint32> optimized_polygon_path;
    if (OptimizePathVisibility(mesh_data, funnel_path, polygon_path,
                               optimized_path, optimized_polygon_path))
    {
        out_path.swap(optimized_path);
        if (out_polygon_path != nullptr)
        {
            // デバッグ表示では、平滑化前にA*が選んだコリドーを残す。
            // これにより、経路線がコリドー外を直進して改善されたことも確認できる。
            *out_polygon_path = polygon_path;
        }
    }
    else
    {
        out_path.swap(funnel_path);
        if (out_polygon_path != nullptr)
        {
            *out_polygon_path = polygon_path;
        }
    }

    return true;
}

bool NavigationMeshQuery::TraceSegmentAcrossNavMesh(
    const NavigationMeshData& mesh_data,
    uint32 start_polygon_index, uint32 goal_polygon_index,
    const Vec3& start_position, const Vec3& goal_position,
    std::vector<Vec3>& out_path,
    std::vector<uint32>* out_polygon_path) const
{
    constexpr float kTraversalEpsilon = 0.0001f;

    struct TraversalState
    {
        uint32 polygon_index = UINT32_MAX;
        float path_t = 0.0f;
        uint32 parent_state_index = UINT32_MAX;
        Vec3 crossing_position;
    };

    out_path.clear();
    if (out_polygon_path != nullptr)
    {
        out_polygon_path->clear();
    }

    if (start_polygon_index >= mesh_data.polygons.size() ||
        goal_polygon_index >= mesh_data.polygons.size())
    {
        return false;
    }

    out_path.push_back(start_position);
    if (start_polygon_index == goal_polygon_index)
    {
        if (out_polygon_path != nullptr)
        {
            out_polygon_path->push_back(start_polygon_index);
        }

        if (!IsSamePointXZ(start_position, goal_position))
        {
            out_path.push_back(goal_position);
        }
        return true;
    }

    const Vec3 path_direction = goal_position - start_position;
    std::vector<TraversalState> traversal_states;
    std::vector<uint32> open_state_indices;
    std::vector<float> furthest_path_t(mesh_data.polygons.size(), -FLT_MAX);
    std::vector<uint32> connected_polygons;

    traversal_states.push_back({start_polygon_index, 0.0f, UINT32_MAX, start_position});

    CollectConnectedPolygonsAtPoint(mesh_data, start_polygon_index,
                                    start_position, connected_polygons);

    const float start_probe_t = (std::min)(1.0f, kTraversalEpsilon);
    const Vec3 start_probe = start_position + path_direction * start_probe_t;
    for (const uint32 polygon_index : connected_polygons)
    {
        if (!IsPointInsidePolygonXZ(mesh_data, polygon_index, start_probe))
        {
            continue;
        }

        if (polygon_index == start_polygon_index)
        {
            open_state_indices.push_back(0);
        }
        else
        {
            traversal_states.push_back({polygon_index, 0.0f, 0, start_position});
            open_state_indices.push_back(static_cast<uint32>(traversal_states.size() - 1));
        }
        furthest_path_t[polygon_index] = 0.0f;
    }

    const size_t max_step_count = mesh_data.polygons.size() * 4;
    size_t step_count = 0;
    while (!open_state_indices.empty() && step_count < max_step_count)
    {
        ++step_count;

        const uint32 current_state_index = open_state_indices.back();
        open_state_indices.pop_back();
        const TraversalState current_state = traversal_states[current_state_index];

        bool reaches_goal_polygon = current_state.polygon_index == goal_polygon_index;
        if (!reaches_goal_polygon &&
            IsPointInsidePolygonXZ(mesh_data, current_state.polygon_index, goal_position))
        {
            CollectConnectedPolygonsAtPoint(mesh_data, current_state.polygon_index,
                                            goal_position, connected_polygons);
            reaches_goal_polygon = std::find(connected_polygons.begin(), connected_polygons.end(),
                                             goal_polygon_index) != connected_polygons.end();
        }

        if (reaches_goal_polygon)
        {
            std::vector<uint32> ordered_state_indices;
            for (uint32 state_index = current_state_index;
                 state_index != UINT32_MAX;
                 state_index = traversal_states[state_index].parent_state_index)
            {
                ordered_state_indices.push_back(state_index);
            }
            std::reverse(ordered_state_indices.begin(), ordered_state_indices.end());

            out_path.clear();
            out_path.push_back(start_position);
            for (const uint32 state_index : ordered_state_indices)
            {
                const Vec3& crossing_position = traversal_states[state_index].crossing_position;
                if ((crossing_position - out_path.back()).Length() > 0.001f)
                {
                    out_path.push_back(crossing_position);
                }
            }
            if ((goal_position - out_path.back()).Length() > 0.001f)
            {
                out_path.push_back(goal_position);
            }

            if (out_polygon_path != nullptr)
            {
                for (const uint32 state_index : ordered_state_indices)
                {
                    out_polygon_path->push_back(traversal_states[state_index].polygon_index);
                }

                if (out_polygon_path->empty() || out_polygon_path->front() != start_polygon_index)
                {
                    out_polygon_path->insert(out_polygon_path->begin(), start_polygon_index);
                }
                if (out_polygon_path->back() != goal_polygon_index)
                {
                    out_polygon_path->push_back(goal_polygon_index);
                }

                const auto unique_end = std::unique(out_polygon_path->begin(), out_polygon_path->end());
                out_polygon_path->erase(unique_end, out_polygon_path->end());
            }
            return true;
        }

        const NavigationMeshPolygon& polygon = mesh_data.polygons[current_state.polygon_index];
        const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
        if (vertex_count < 3 || polygon.neighbor_polygon_indices.size() != vertex_count)
        {
            continue;
        }

        float nearest_path_t = FLT_MAX;
        Vec3 nearest_crossing_position;

        for (uint32 edge_index = 0; edge_index < vertex_count; ++edge_index)
        {
            const uint32 next_edge_index = (edge_index + 1) % vertex_count;
            const uint32 edge_start_index = polygon.vertex_indices[edge_index];
            const uint32 edge_end_index = polygon.vertex_indices[next_edge_index];
            if (edge_start_index >= mesh_data.vertices.size() ||
                edge_end_index >= mesh_data.vertices.size())
            {
                continue;
            }

            float path_t = 0.0f;
            float edge_t = 0.0f;
            if (!CalcSegmentIntersectionXZ(start_position, goal_position,
                                           mesh_data.vertices[edge_start_index],
                                           mesh_data.vertices[edge_end_index], path_t, edge_t) ||
                path_t <= current_state.path_t + kTraversalEpsilon)
            {
                continue;
            }

            if (path_t < nearest_path_t)
            {
                nearest_path_t = path_t;
                const float clamped_edge_t = (std::max)(0.0f, (std::min)(1.0f, edge_t));
                const Vec3& edge_start = mesh_data.vertices[edge_start_index];
                const Vec3& edge_end = mesh_data.vertices[edge_end_index];
                nearest_crossing_position =
                    edge_start + (edge_end - edge_start) * clamped_edge_t;
            }
        }

        if (nearest_path_t == FLT_MAX)
        {
            continue;
        }

        const Vec3 crossing_position = start_position + path_direction * nearest_path_t;
        CollectConnectedPolygonsAtPoint(mesh_data, current_state.polygon_index,
                                        crossing_position, connected_polygons);

        const float probe_t = (std::min)(1.0f, nearest_path_t + kTraversalEpsilon);
        const Vec3 probe_position = start_position + path_direction * probe_t;
        for (const uint32 polygon_index : connected_polygons)
        {
            if (polygon_index >= mesh_data.polygons.size() ||
                !IsPointInsidePolygonXZ(mesh_data, polygon_index, probe_position) ||
                nearest_path_t <= furthest_path_t[polygon_index] + kTraversalEpsilon)
            {
                continue;
            }

            furthest_path_t[polygon_index] = nearest_path_t;
            traversal_states.push_back(
                {polygon_index, nearest_path_t, current_state_index, nearest_crossing_position});
            open_state_indices.push_back(static_cast<uint32>(traversal_states.size() - 1));
        }
    }

    out_path.clear();
    return false;
}

bool NavigationMeshQuery::OptimizePathVisibility(
    const NavigationMeshData& mesh_data,
    const std::vector<Vec3>& path,
    const std::vector<uint32>& polygon_path,
    std::vector<Vec3>& out_path,
    std::vector<uint32>& out_polygon_path) const
{
    out_path.clear();
    out_polygon_path.clear();
    if (path.empty() || polygon_path.empty())
    {
        return false;
    }

    if (path.size() == 1)
    {
        out_path = path;
        out_polygon_path.push_back(polygon_path.front());
        return true;
    }

    std::vector<uint32> point_polygon_indices(path.size(), UINT32_MAX);
    point_polygon_indices.front() = polygon_path.front();
    point_polygon_indices.back() = polygon_path.back();

    size_t minimum_corridor_index = 0;
    for (size_t point_index = 1; point_index + 1 < path.size(); ++point_index)
    {
        float best_distance_squared = FLT_MAX;
        size_t best_corridor_index = minimum_corridor_index;

        for (size_t corridor_index = minimum_corridor_index;
             corridor_index < polygon_path.size(); ++corridor_index)
        {
            const uint32 polygon_index = polygon_path[corridor_index];
            if (polygon_index >= mesh_data.polygons.size())
            {
                continue;
            }

            const NavigationMeshPolygon& polygon = mesh_data.polygons[polygon_index];
            if (polygon.vertex_indices.size() < 3)
            {
                continue;
            }

            const uint32 first_vertex_index = polygon.vertex_indices[0];
            if (first_vertex_index >= mesh_data.vertices.size())
            {
                continue;
            }

            float polygon_distance_squared = FLT_MAX;
            for (size_t triangle_index = 1;
                 triangle_index + 1 < polygon.vertex_indices.size(); ++triangle_index)
            {
                const uint32 second_vertex_index = polygon.vertex_indices[triangle_index];
                const uint32 third_vertex_index = polygon.vertex_indices[triangle_index + 1];
                if (second_vertex_index >= mesh_data.vertices.size() ||
                    third_vertex_index >= mesh_data.vertices.size())
                {
                    continue;
                }

                const Vec3 closest_point = CalcClosestPointOnTriangle(
                    path[point_index],
                    mesh_data.vertices[first_vertex_index],
                    mesh_data.vertices[second_vertex_index],
                    mesh_data.vertices[third_vertex_index]);
                polygon_distance_squared = (std::min)(
                    polygon_distance_squared,
                    (path[point_index] - closest_point).LengthSquared());
            }

            const bool is_closer = polygon_distance_squared < best_distance_squared - kEpsilon;
            const bool is_later_equal_candidate =
                std::abs(polygon_distance_squared - best_distance_squared) <= kEpsilon &&
                corridor_index > best_corridor_index;
            if (!is_closer && !is_later_equal_candidate)
            {
                continue;
            }

            best_distance_squared = polygon_distance_squared;
            best_corridor_index = corridor_index;
        }

        if (best_distance_squared == FLT_MAX)
        {
            return false;
        }

        point_polygon_indices[point_index] = polygon_path[best_corridor_index];
        minimum_corridor_index = best_corridor_index;
    }

    size_t current_point_index = 0;
    while (current_point_index + 1 < path.size())
    {
        bool found_visible_point = false;

        for (size_t candidate_point_index = path.size() - 1;
             candidate_point_index > current_point_index; --candidate_point_index)
        {
            std::vector<Vec3> segment_path;
            std::vector<uint32> segment_polygon_path;
            if (!TraceSegmentAcrossNavMesh(
                    mesh_data,
                    point_polygon_indices[current_point_index],
                    point_polygon_indices[candidate_point_index],
                    path[current_point_index], path[candidate_point_index],
                    segment_path, &segment_polygon_path))
            {
                continue;
            }

            for (const Vec3& point : segment_path)
            {
                if (out_path.empty() || (point - out_path.back()).Length() > 0.001f)
                {
                    out_path.push_back(point);
                }
            }

            for (const uint32 polygon_index : segment_polygon_path)
            {
                if (out_polygon_path.empty() || out_polygon_path.back() != polygon_index)
                {
                    out_polygon_path.push_back(polygon_index);
                }
            }

            current_point_index = candidate_point_index;
            found_visible_point = true;
            break;
        }

        if (!found_visible_point)
        {
            out_path.clear();
            out_polygon_path.clear();
            return false;
        }
    }

    return !out_path.empty() && !out_polygon_path.empty();
}

Vec3 NavigationMeshQuery::CalcPolygonCenter(const NavigationMeshData& mesh_data, uint32 polygon_index) const
{
    const auto& polygon = mesh_data.polygons[polygon_index];
    if (polygon.vertex_indices.size() < 3)
    {
        return {};
    }
    Vec3 total_vertex;
    for (uint32 vertex_index : polygon.vertex_indices)
    {
        total_vertex += mesh_data.vertices[vertex_index];
    }

    Vec3 center = total_vertex / static_cast<float>(polygon.vertex_indices.size());
    return center;
}

Vec3 NavigationMeshQuery::CalcClosestPointOnTriangle(const Vec3& point, const Vec3& a, const Vec3& b,
                                                      const Vec3& c) const
{
    const Vec3 ab = b - a;
    const Vec3 ac = c - a;
    const Vec3 ap = point - a;
    const float d1 = Dot(ab, ap);
    const float d2 = Dot(ac, ap);

    if (d1 <= 0.0f && d2 <= 0.0f)
    {
        return a;
    }

    const Vec3 bp = point - b;
    const float d3 = Dot(ab, bp);
    const float d4 = Dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3)
    {
        return b;
    }

    const float vc = d1 * d4 - d3 * d2;

    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
    {
        const float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    const Vec3 cp = point - c;
    const float d5 = Dot(ab, cp);
    const float d6 = Dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6)
    {
        return c;
    }

    const float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
    {
        const float v = d2 / (d2 - d6);
        return a + v * ac;
    }

    const float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
    {
        const float v = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + v * (c - b);
    }
    const float denom = 1.0f / (va + vb + vc);
    const float v = vb * denom;
    const float w = vc * denom;
    return a + ab * v + ac * w;
}

Vec3 NavigationMeshQuery::CalcBestPortalPoint(const Vec3& current_position, const Vec3& goal_position,
                                              const Vec3& edge_start, const Vec3& edge_end) const
{
    constexpr uint32 kSearchIterationCount = 16;

    float min_t = 0.0f;
    float max_t = 1.0f;
    const Vec3 edge = edge_end - edge_start;

    const auto calc_cost = [&](float t)
    {
        const Vec3 point = edge_start + edge * t;
        return (point - current_position).Length() + (goal_position - point).Length();
    };

    for (uint32 iteration = 0; iteration < kSearchIterationCount; ++iteration)
    {
        const float first_t = (min_t * 2.0f + max_t) / 3.0f;
        const float second_t = (min_t + max_t * 2.0f) / 3.0f;
        if (calc_cost(first_t) <= calc_cost(second_t))
        {
            max_t = second_t;
        }
        else
        {
            min_t = first_t;
        }
    }

    return edge_start + edge * ((min_t + max_t) * 0.5f);
}

bool NavigationMeshQuery::TryGetSharedEdge(const NavigationMeshData& mesh_data, uint32 first_polygon_index,
                                           uint32 second_polygon_index, Vec3& out_a, Vec3& out_b) const
{
    if (first_polygon_index >= mesh_data.polygons.size() ||
        second_polygon_index >= mesh_data.polygons.size())
    {
        return false;
    }

    const NavigationMeshPolygon& first_polygon = mesh_data.polygons[first_polygon_index];
    const NavigationMeshPolygon& second_polygon = mesh_data.polygons[second_polygon_index];
    const uint32 first_vertex_count = static_cast<uint32>(first_polygon.vertex_indices.size());
    const uint32 second_vertex_count = static_cast<uint32>(second_polygon.vertex_indices.size());
    if (first_vertex_count < 3 || second_vertex_count < 3)
    {
        return false;
    }

    for (uint32 first_edge_index = 0; first_edge_index < first_vertex_count; ++first_edge_index)
    {
        const uint32 first_start_index = first_polygon.vertex_indices[first_edge_index];
        const uint32 first_end_index = first_polygon.vertex_indices[(first_edge_index + 1) % first_vertex_count];
        if (first_start_index >= mesh_data.vertices.size() || first_end_index >= mesh_data.vertices.size())
        {
            return false;
        }

        for (uint32 second_edge_index = 0; second_edge_index < second_vertex_count; ++second_edge_index)
        {
            const uint32 second_start_index = second_polygon.vertex_indices[second_edge_index];
            const uint32 second_end_index =
                second_polygon.vertex_indices[(second_edge_index + 1) % second_vertex_count];

            // 隣接ポリゴンでは共有辺の向きが逆になる。
            if (first_start_index != second_end_index || first_end_index != second_start_index)
            {
                continue;
            }

            // first_polygon側の頂点順を保って返す。
            out_a = mesh_data.vertices[first_start_index];
            out_b = mesh_data.vertices[first_end_index];
            return true;
        }
    }

    return false;
}

bool NavigationMeshQuery::BuildPortals(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                                       const Vec3& start_pos, const Vec3& goal_pos,
                                       std::vector<Portal>& out_portals) const
{
    out_portals.clear();

    if (polygon_path.empty())
    {
        return false;
    }

    for (uint32 poly : polygon_path)
    {
        if (poly >= mesh_data.polygons.size())
        {
            return false;
        }
    }

    out_portals.reserve(polygon_path.size() + 1);
    Portal start_portal;
    start_portal.left = start_pos;
    start_portal.right = start_pos;
    out_portals.push_back(start_portal);

    for (uint32 i = 0; i + 1 < polygon_path.size(); ++i)
    {
        const uint32 cur_poly = polygon_path[i];
        const uint32 next_poly = polygon_path[i + 1];

        Vec3 edge_a, edge_b;

        if (!TryGetSharedEdge(mesh_data, cur_poly, next_poly, edge_a, edge_b))
        {
            out_portals.clear();
            return false;
        }

        Portal portal;
        // ポリゴンはXZ平面で反時計回り。現在ポリゴンの共有辺は
        // 進行方向から見て right -> left の順に並ぶ。
        portal.right = edge_a;
        portal.left = edge_b;

        out_portals.push_back(portal);
    }

    Portal goal_portal;
    goal_portal.left = goal_pos;
    goal_portal.right = goal_pos;
    out_portals.push_back(goal_portal);

    return true;
}

bool NavigationMeshQuery::CalcSegmentIntersectionXZ(const Vec3& path_start, const Vec3& path_end,
                                                    const Vec3& edge_start, const Vec3& edge_end,
                                                    float& out_path_t, float& out_edge_t) const
{
    const float path_x = path_end.x - path_start.x;
    const float path_z = path_end.z - path_start.z;
    const float edge_x = edge_end.x - edge_start.x;
    const float edge_z = edge_end.z - edge_start.z;
    const float denominator = path_x * edge_z - path_z * edge_x;
    if (std::abs(denominator) <= kEpsilon)
    {
        return false;
    }

    const float offset_x = edge_start.x - path_start.x;
    const float offset_z = edge_start.z - path_start.z;
    out_path_t = (offset_x * edge_z - offset_z * edge_x) / denominator;
    out_edge_t = (offset_x * path_z - offset_z * path_x) / denominator;

    return out_path_t >= -kEpsilon && out_path_t <= 1.0f + kEpsilon &&
        out_edge_t >= -kEpsilon && out_edge_t <= 1.0f + kEpsilon;
}

bool NavigationMeshQuery::IsPointInsidePolygonXZ(const NavigationMeshData& mesh_data, uint32 polygon_index,
                                                 const Vec3& point) const
{
    if (polygon_index >= mesh_data.polygons.size())
    {
        return false;
    }

    const NavigationMeshPolygon& polygon = mesh_data.polygons[polygon_index];
    const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
    if (vertex_count < 3)
    {
        return false;
    }

    bool has_positive_area = false;
    bool has_negative_area = false;
    for (uint32 edge_index = 0; edge_index < vertex_count; ++edge_index)
    {
        const uint32 edge_start_index = polygon.vertex_indices[edge_index];
        const uint32 edge_end_index = polygon.vertex_indices[(edge_index + 1) % vertex_count];
        if (edge_start_index >= mesh_data.vertices.size() ||
            edge_end_index >= mesh_data.vertices.size())
        {
            return false;
        }

        const float area = CalcSignedAreaXZ(mesh_data.vertices[edge_start_index],
                                            mesh_data.vertices[edge_end_index], point);
        has_positive_area |= area > kEpsilon;
        has_negative_area |= area < -kEpsilon;
        if (has_positive_area && has_negative_area)
        {
            return false;
        }
    }

    return true;
}

void NavigationMeshQuery::CollectConnectedPolygonsAtPoint(
    const NavigationMeshData& mesh_data, uint32 start_polygon_index,
    const Vec3& point, std::vector<uint32>& out_polygon_indices) const
{
    out_polygon_indices.clear();
    if (start_polygon_index >= mesh_data.polygons.size())
    {
        return;
    }

    std::vector<bool> visited(mesh_data.polygons.size(), false);
    std::vector<uint32> open_polygons;
    open_polygons.push_back(start_polygon_index);
    visited[start_polygon_index] = true;

    while (!open_polygons.empty())
    {
        const uint32 polygon_index = open_polygons.back();
        open_polygons.pop_back();
        out_polygon_indices.push_back(polygon_index);

        const NavigationMeshPolygon& polygon = mesh_data.polygons[polygon_index];
        const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
        if (vertex_count < 3 || polygon.neighbor_polygon_indices.size() != vertex_count)
        {
            continue;
        }

        for (uint32 edge_index = 0; edge_index < vertex_count; ++edge_index)
        {
            const uint32 neighbor_index = polygon.neighbor_polygon_indices[edge_index];
            const uint32 edge_start_index = polygon.vertex_indices[edge_index];
            const uint32 edge_end_index = polygon.vertex_indices[(edge_index + 1) % vertex_count];
            if (neighbor_index == UINT32_MAX ||
                neighbor_index >= mesh_data.polygons.size() ||
                visited[neighbor_index] ||
                edge_start_index >= mesh_data.vertices.size() ||
                edge_end_index >= mesh_data.vertices.size() ||
                !IsPointOnSegmentXZ(point, mesh_data.vertices[edge_start_index],
                                    mesh_data.vertices[edge_end_index]))
            {
                continue;
            }

            visited[neighbor_index] = true;
            open_polygons.push_back(neighbor_index);
        }
    }
}

bool NavigationMeshQuery::IsPointOnSegmentXZ(const Vec3& point, const Vec3& start, const Vec3& end) const
{
    constexpr float kPointEpsilon = 0.001f;
    constexpr float kPointEpsilonSquared = kPointEpsilon * kPointEpsilon;

    const float edge_x = end.x - start.x;
    const float edge_z = end.z - start.z;
    const float edge_length_squared = edge_x * edge_x + edge_z * edge_z;
    if (edge_length_squared <= kPointEpsilonSquared)
    {
        const float diff_x = point.x - start.x;
        const float diff_z = point.z - start.z;
        return diff_x * diff_x + diff_z * diff_z <= kPointEpsilonSquared;
    }

    const float point_x = point.x - start.x;
    const float point_z = point.z - start.z;
    const float segment_t = (point_x * edge_x + point_z * edge_z) / edge_length_squared;
    if (segment_t < -kPointEpsilon || segment_t > 1.0f + kPointEpsilon)
    {
        return false;
    }

    const float closest_x = start.x + edge_x * segment_t;
    const float closest_z = start.z + edge_z * segment_t;
    const float diff_x = point.x - closest_x;
    const float diff_z = point.z - closest_z;
    return diff_x * diff_x + diff_z * diff_z <= kPointEpsilonSquared;
}

float NavigationMeshQuery::CalcSignedAreaXZ(const Vec3& a, const Vec3& b, const Vec3& c) const
{
    const float ab_x = b.x - a.x;
    const float ab_z = b.z - a.z;

    const float ac_x = c.x - a.x;
    const float ac_z = c.z - a.z;

    return ab_x * ac_z - ab_z * ac_x;
}

bool NavigationMeshQuery::IsSamePointXZ(const Vec3& a, const Vec3& b) const
{
    const float diff_x = a.x - b.x;
    const float diff_z = a.z - b.z;
    constexpr float kEpsilonSquared = 0.0001f;

    return diff_x * diff_x + diff_z * diff_z <= kEpsilonSquared;
}

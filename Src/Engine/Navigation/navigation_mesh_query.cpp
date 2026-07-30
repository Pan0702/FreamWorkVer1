#include "navigation_mesh_query.h"
#include <cfloat>
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

    std::vector<Node> nodes;
    std::vector<uint32> open;
    bool find_path = false;
    const Vec3 goal_poly_center = CalcPolygonCenter(mesh_data, goal_poly);

    for (uint32 p = 0; p < mesh_data.polygons.size(); ++p)
    {
        Node node;
        node.h_cost = (goal_poly_center - CalcPolygonCenter(mesh_data, p)).Length();
        if (p == start_poly)
        {
            node.g_cost = 0.0f;
            node.f_cost = node.h_cost + node.g_cost;
            node.state = State::kOpen;
        }

        nodes.push_back(node);
    }

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

            const float g_cost = (CalcPolygonCenter(mesh_data, neighbor_index) -
                CalcPolygonCenter(mesh_data, cur_index)).Length() + nodes[cur_index].g_cost;
            if (g_cost < nodes[neighbor_index].g_cost || nodes[neighbor_index].state == State::kUnvisited)
            {
                nodes[neighbor_index].parent_index = cur_index;
                nodes[neighbor_index].g_cost = g_cost;
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

        std::reverse(out_poly.begin(), out_poly.end());
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

    if (!BuildPortals(
        mesh_data,
        polygon_path,
        start_position,
        goal_pos,
        portals))
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
        const Vec3 new_left =
            portals[portal_index].left;

        const Vec3 new_right =
            portals[portal_index].right;

        // 右側を狭められるか確認
        if (CalcSignedAreaXZ(apex, right, new_right) <= 0.0f)
        {
            if (IsSamePointXZ(apex, right) ||
                CalcSignedAreaXZ(apex, left, new_right) > 0.0f)
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
        if (CalcSignedAreaXZ(apex, left, new_left) >= 0.0f)
        {
            if (IsSamePointXZ(apex, left) ||
                CalcSignedAreaXZ(apex, right, new_left) < 0.0f)
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
                                   const Vec3& goal_pos, std::vector<Vec3>& out_path) const
{
    out_path.clear();

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

    std::vector<uint32> polygon_path;

    if (!FindPolygonPath(mesh_data, start_polygon_index, goal_polygon_index, polygon_path))
    {
        return false;
    }

    if (!FindStraightPath(mesh_data, polygon_path, nearest_start_position, nearest_goal_position, out_path))
    {
        out_path.clear();
        return false;
    }

    return true;
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

bool NavigationMeshQuery::TryGetSharedEdge(const NavigationMeshData& mesh_data, uint32 first_polygon_index,
                                           uint32 second_polygon_index, Vec3& out_a, Vec3& out_b) const
{
    if (first_polygon_index >= mesh_data.polygons.size() ||
        second_polygon_index >= mesh_data.polygons.size())
    {
        return false;
    }

    const auto& first_polygon =
        mesh_data.polygons[first_polygon_index];

    const auto& second_polygon =
        mesh_data.polygons[second_polygon_index];

    uint32 shared_vertex_indices[2] =
    {
        UINT32_MAX,
        UINT32_MAX
    };

    uint32 shared_vertex_count = 0;

    for (uint32 first_vertex_index : first_polygon.vertex_indices)
    {
        for (uint32 second_vertex_index : second_polygon.vertex_indices)
        {
            if (first_vertex_index != second_vertex_index)
            {
                continue;
            }

            if (shared_vertex_count > 0 &&
                shared_vertex_indices[0] == first_vertex_index)
            {
                continue;
            }

            if (shared_vertex_count >= 2)
            {
                return false;
            }

            shared_vertex_indices[shared_vertex_count] =
                first_vertex_index;

            ++shared_vertex_count;
        }
    }

    if (shared_vertex_count != 2)
    {
        return false;
    }

    if (shared_vertex_indices[0] >= mesh_data.vertices.size() ||
        shared_vertex_indices[1] >= mesh_data.vertices.size())
    {
        return false;
    }

    out_a = mesh_data.vertices[shared_vertex_indices[0]];
    out_b = mesh_data.vertices[shared_vertex_indices[1]];

    return true;
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

        const Vec3 cur_center = CalcPolygonCenter(mesh_data, cur_poly);
        const Vec3 next_center = CalcPolygonCenter(mesh_data, next_poly);


        const Vec3 direction = next_center - cur_center;
        const Vec3 to_a = edge_a - cur_center;
        const Vec3 to_b = edge_b - cur_center;

        const float side_a = direction.x * to_a.z - direction.z * to_a.x;
        const float side_b = direction.x * to_b.z - direction.z * to_b.x;

        Portal portal;

        if (side_a >= side_b)
        {
            portal.left = edge_a;
            portal.right = edge_b;
        }
        else
        {
            portal.left = edge_b;
            portal.right = edge_a;
        }

        out_portals.push_back(portal);
    }

    Portal goal_portal;
    goal_portal.left = goal_pos;
    goal_portal.right = goal_pos;
    out_portals.push_back(goal_portal);

    return true;
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

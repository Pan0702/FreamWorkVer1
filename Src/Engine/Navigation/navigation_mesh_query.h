#pragma once
#include "navigation_type.h"
#include "../../Core/Math/my_math.h"

class NavigationMeshQuery
{
public:
    bool FindNearestPolygon(const NavigationMeshData& mesh_data,
                            const Vec3& position, uint32& out_polygon_index) const;
    bool FindPolygonPath(const NavigationMeshData& mesh_data, uint32 start_poly, uint32 goal_poly,
                         std::vector<uint32>& out_poly) const;
    bool FindStraightPath(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                          const Vec3& start_pos, const Vec3& goal_pos, std::vector<Vec3>& out_path) const;
    bool FindNearestPolygon(const NavigationMeshData& mesh_data, const Vec3& position,
                            uint32& out_polygon_index, Vec3& out_closest_point) const;
    bool FindPath(const NavigationMeshData& mesh_data, const Vec3& start_pos, const Vec3& goal_position,
                  std::vector<Vec3>& out_path) const;

private:
    enum class State : uint8
    {
        kUnvisited,
        kOpen,
        kClosed,
    };

    struct Node
    {
        float g_cost = FLT_MAX;
        float h_cost = 0.0f;
        float f_cost = FLT_MAX;

        uint32 parent_index = UINT32_MAX;
        State state = State::kUnvisited;
    };

    struct Portal
    {
        Vec3 left;
        Vec3 right;
    };

    Vec3 CalcPolygonCenter(const NavigationMeshData& mesh_data, uint32 polygon_index) const;
    Vec3 CalcClosestPointOnTriangle(const Vec3& point, const Vec3& a, const Vec3& b, const Vec3& c) const;
    bool TryGetSharedEdge(const NavigationMeshData& mesh_data, uint32 first_polygon_index,
                          uint32 second_polygon_index, Vec3& out_a, Vec3& out_b) const;
    bool BuildPortals(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                      const Vec3& start_pos, const Vec3& goal_pos, std::vector<Portal>& out_portals) const;
    float CalcSignedAreaXZ(const Vec3& a, const Vec3& b, const Vec3& c) const;
    bool IsSamePointXZ(const Vec3& a, const Vec3& b) const;
};

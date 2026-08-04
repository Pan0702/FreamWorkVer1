#pragma once
#include "navigation_type.h"
#include "../../Core/Math/my_math.h"

class NavigationMeshQuery
{
public:
    bool FindNearestPolygon(const NavigationMeshData& mesh_data,
                            const Vec3& position, uint32& out_polygon_index) const;
    bool FindPolygonPath(const NavigationMeshData& mesh_data, uint32 start_poly, uint32 goal_poly,
                         const Vec3& start_position, const Vec3& goal_position,
                         std::vector<uint32>& out_poly) const;
    bool FindStraightPath(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                          const Vec3& start_pos, const Vec3& goal_pos, std::vector<Vec3>& out_path) const;
    bool FindNearestPolygon(const NavigationMeshData& mesh_data, const Vec3& position,
                            uint32& out_polygon_index, Vec3& out_closest_point) const;
    bool FindPath(const NavigationMeshData& mesh_data, const Vec3& start_pos, const Vec3& goal_position,
                  std::vector<Vec3>& out_path,
                  std::vector<uint32>* out_polygon_path = nullptr) const;

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

        Vec3 position;
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
    Vec3 CalcBestPortalPoint(const Vec3& current_position, const Vec3& goal_position,
                             const Vec3& edge_start, const Vec3& edge_end) const;
    bool TryGetSharedEdge(const NavigationMeshData& mesh_data, uint32 first_polygon_index,
                          uint32 second_polygon_index, Vec3& out_a, Vec3& out_b) const;
    bool BuildPortals(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                      const Vec3& start_pos, const Vec3& goal_pos, std::vector<Portal>& out_portals) const;
    bool TraceSegmentAcrossNavMesh(const NavigationMeshData& mesh_data,
                                   uint32 start_polygon_index, uint32 goal_polygon_index,
                                   const Vec3& start_position, const Vec3& goal_position,
                                   std::vector<Vec3>& out_path,
                                   std::vector<uint32>* out_polygon_path) const;
    bool OptimizePathVisibility(const NavigationMeshData& mesh_data,
                                const std::vector<Vec3>& path,
                                const std::vector<uint32>& polygon_path,
                                std::vector<Vec3>& out_path,
                                std::vector<uint32>& out_polygon_path) const;
    bool CalcSegmentIntersectionXZ(const Vec3& path_start, const Vec3& path_end,
                                   const Vec3& edge_start, const Vec3& edge_end,
                                   float& out_path_t, float& out_edge_t) const;
    bool IsPointInsidePolygonXZ(const NavigationMeshData& mesh_data, uint32 polygon_index,
                                const Vec3& point) const;
    void CollectConnectedPolygonsAtPoint(const NavigationMeshData& mesh_data, uint32 start_polygon_index,
                                         const Vec3& point, std::vector<uint32>& out_polygon_indices) const;
    bool IsPointOnSegmentXZ(const Vec3& point, const Vec3& start, const Vec3& end) const;
    float CalcSignedAreaXZ(const Vec3& a, const Vec3& b, const Vec3& c) const;
    bool IsSamePointXZ(const Vec3& a, const Vec3& b) const;
};

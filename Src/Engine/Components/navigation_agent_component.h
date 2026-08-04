#pragma once

#include <vector>

#include "../component.h"
#include "../../Core/Math/my_math.h"

class NavigationSystem;

class NavigationAgentComponent : public Component
{
public:
    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    void Tick(float dt) override;

    bool SetDestination(const Vec3& destination);
    void ClearPath();

    const std::vector<Vec3>& GetPath() const;
    const std::vector<uint32>& GetCorridorPolygonIndices() const;
    uint32 GetCurrentPathIndex() const;
    bool HasPath() const;

    void SetDebugDrawEnabled(bool enabled);
    bool IsDebugDrawEnabled() const;
    void SetDebugDrawCorridorEnabled(bool enabled);
    bool IsDebugDrawCorridorEnabled() const;
    void SetDebugDrawColor(const Vec4& color);
    void SetDebugDrawHeightOffset(float height_offset);
    void SetDebugPointRadius(float radius);
    bool TryGetCurrentWaypoint(Vec3& out_waypoint) const;
    void AdvanceWaypointIfReached(const Vec3& position, float radius);

private:
    void DrawDebug() const;

    NavigationSystem* navigation_system_ = nullptr;
    std::vector<Vec3> path_;
    std::vector<uint32> corridor_polygon_indices_;
    uint32 current_path_index_ = 0;

    bool debug_draw_enabled_ = false;
    bool debug_draw_corridor_enabled_ = false;
    Vec4 debug_draw_color_ = Vec4(1.0f, 0.0f, 1.0f, 1.0f);
    float debug_draw_height_offset_ = 2.0f;
    float debug_point_radius_ = 3.0f;
};

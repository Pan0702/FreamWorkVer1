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
    uint32 GetCurrentPathIndex() const;
    bool HasPath() const;

    void SetDebugDrawEnabled(bool enabled);
    bool IsDebugDrawEnabled() const;
    void SetDebugDrawColor(const Vec4& color);
    void SetDebugDrawHeightOffset(float height_offset);
    void SetDebugPointRadius(float radius);

private:
    void DrawDebug() const;

    NavigationSystem* navigation_system_ = nullptr;
    std::vector<Vec3> path_;
    uint32 current_path_index_ = 0;

    bool debug_draw_enabled_ = false;
    Vec4 debug_draw_color_ = Vec4(1.0f, 0.0f, 1.0f, 1.0f);
    float debug_draw_height_offset_ = 2.0f;
    float debug_point_radius_ = 3.0f;
};

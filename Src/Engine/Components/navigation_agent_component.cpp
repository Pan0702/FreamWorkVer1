#include "navigation_agent_component.h"

#include "../actor.h"
#include "../attach_context.h"
#include "../Navigation/navigation_system.h"
#include "../../Debug/debug.h"
#include "../../Debug/navigation_debug_renderer.h"

void NavigationAgentComponent::OnAttach(const AttachContext& context)
{
    navigation_system_ = context.navigation_system;
    Component::OnAttach(context);
}

void NavigationAgentComponent::OnDetach()
{
    navigation_system_ = nullptr;
    ClearPath();
    Component::OnDetach();
}

void NavigationAgentComponent::Tick(float dt)
{
    // 経路の追従は利用側が行うため、ここではデバッグ描画しかすることがない。
    if (debug_draw_enabled_)
    {
        DrawDebug();
    }

    Component::Tick(dt);
}

bool NavigationAgentComponent::SetDestination(const Vec3& destination)
{
    ClearPath();

    const Actor* owner = GetOwner();
    if (navigation_system_ == nullptr || owner == nullptr)
    {
        return false;
    }

    // 経路の始点は常に所有アクターの現在位置になる。
    const Vec3 start_position = owner->GetTransform().position;
    if (!navigation_system_->FindPath(start_position, destination,
                                      path_, &corridor_polygon_indices_))
    {
        ClearPath();
        return false;
    }

    // path_[0]は今いる場所なので、目指す先は次の点から始める。
    // 経路が1点しかない場合は、その点自体を目標にする。
    current_path_index_ = path_.size() > 1 ? 1u : 0u;
    return true;
}

void NavigationAgentComponent::ClearPath()
{
    path_.clear();
    corridor_polygon_indices_.clear();
    current_path_index_ = 0;
}

const std::vector<Vec3>& NavigationAgentComponent::GetPath() const
{
    return path_;
}

const std::vector<uint32>& NavigationAgentComponent::GetCorridorPolygonIndices() const
{
    return corridor_polygon_indices_;
}

uint32 NavigationAgentComponent::GetCurrentPathIndex() const
{
    return current_path_index_;
}

bool NavigationAgentComponent::HasPath() const
{
    return !path_.empty();
}

void NavigationAgentComponent::SetDebugDrawEnabled(bool enabled)
{
    debug_draw_enabled_ = enabled;
}

bool NavigationAgentComponent::IsDebugDrawEnabled() const
{
    return debug_draw_enabled_;
}

void NavigationAgentComponent::SetDebugDrawCorridorEnabled(bool enabled)
{
    debug_draw_corridor_enabled_ = enabled;
}

bool NavigationAgentComponent::IsDebugDrawCorridorEnabled() const
{
    return debug_draw_corridor_enabled_;
}

void NavigationAgentComponent::SetDebugDrawColor(const Vec4& color)
{
    debug_draw_color_ = color;
}

void NavigationAgentComponent::SetDebugDrawHeightOffset(float height_offset)
{
    debug_draw_height_offset_ = height_offset;
}

void NavigationAgentComponent::SetDebugPointRadius(float radius)
{
    debug_point_radius_ = radius;
}

bool NavigationAgentComponent::TryGetCurrentWaypoint(Vec3& out_waypoint) const
{
    // 走破するとインデックスが経路の要素数と等しくなる。それを終了の合図にしている。
    if (current_path_index_ >= path_.size())
    {
        return false;   // 経路なし or 走破済み
    }
    out_waypoint = path_[current_path_index_];
    return true;
}

void NavigationAgentComponent::AdvanceWaypointIfReached(const Vec3& position, float radius)
{
    const float radius_sq = radius * radius;
    // 曲がり角が密集していると1フレームで複数点に到達しうるため、まとめて消化する。
    while (current_path_index_ < path_.size())
    {
        const Vec3& way_point = path_[current_path_index_];
        // 到達判定はXZ平面のみ。足元の高さと経路点の高さは必ずしも一致しないため、
        // 高さを含めると坂や階段で永久に到達できなくなる。
        const float dx = way_point.x - position.x;
        const float dz = way_point.z - position.z;
        if (dx * dx + dz * dz > radius_sq)
        {
            break;
        }
        ++current_path_index_;
    }
}

void NavigationAgentComponent::DrawDebug() const
{
    if (path_.empty())
    {
        return;
    }

    // 経路は地面と同じ高さにあるため、そのまま描くと地形に埋もれて見えない。
    const Vec3 draw_offset(0.0f, debug_draw_height_offset_, 0.0f);

    if (debug_draw_corridor_enabled_ &&
        navigation_system_ != nullptr &&
        !corridor_polygon_indices_.empty())
    {
        // コリドーは経路線より低い位置に描き、線が隠れないようにする。
        NavigationDebugRenderer renderer;
        renderer.DrawCorridor(
            navigation_system_->GetMeshData(),
            corridor_polygon_indices_,
            debug_draw_height_offset_ * 0.5f);
    }

    for (uint32 i = 0; i + 1 < path_.size(); ++i)
    {
        Debug::Get().DrawLine3D(
            path_[i] + draw_offset,
            path_[i + 1] + draw_offset,
            debug_draw_color_);
    }

    // 始点は緑、終点は赤で示す。
    Debug::Get().DrawSphere3D(
        path_.front() + draw_offset,
        debug_point_radius_,
        Vec4(0.0f, 1.0f, 0.0f, 1.0f));

    Debug::Get().DrawSphere3D(
        path_.back() + draw_offset,
        debug_point_radius_,
        Vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

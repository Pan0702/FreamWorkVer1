#include "navigation_agent_component.h"

#include "../actor.h"
#include "../attach_context.h"
#include "../Navigation/navigation_system.h"
#include "../../Debug/debug.h"

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

    const Vec3 start_position = owner->GetTransform().position;
    if (!navigation_system_->FindPath(start_position, destination, path_))
    {
        ClearPath();
        return false;
    }

    current_path_index_ = path_.size() > 1 ? 1u : 0u;
    return true;
}

void NavigationAgentComponent::ClearPath()
{
    path_.clear();
    current_path_index_ = 0;
}

const std::vector<Vec3>& NavigationAgentComponent::GetPath() const
{
    return path_;
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

void NavigationAgentComponent::DrawDebug() const
{
    if (path_.empty())
    {
        return;
    }

    const Vec3 draw_offset(0.0f, debug_draw_height_offset_, 0.0f);

    for (uint32 i = 0; i + 1 < path_.size(); ++i)
    {
        Debug::Get().DrawLine3D(
            path_[i] + draw_offset,
            path_[i + 1] + draw_offset,
            debug_draw_color_);
    }

    Debug::Get().DrawSphere3D(
        path_.front() + draw_offset,
        debug_point_radius_,
        Vec4(0.0f, 1.0f, 0.0f, 1.0f));

    Debug::Get().DrawSphere3D(
        path_.back() + draw_offset,
        debug_point_radius_,
        Vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

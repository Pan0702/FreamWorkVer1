#include "navigation_source_component.h"

#include "../attach_context.h"
#include "../Navigation/navigation_geometry.h"
#include "../Navigation/navigation_system.h"

NavigationSourceComponent::NavigationSourceComponent(MeshColliderComponent* mesh)
{
    mesh_ = mesh;
}

void NavigationSourceComponent::OnAttach(const AttachContext& context)
{
    navigation_system_ = context.navigation_system;
    if (navigation_system_)
    {
        source_id_ = navigation_system_->RegisterSource(this);
    }
    Component::OnAttach(context);
}

void NavigationSourceComponent::OnDetach()
{
    if (navigation_system_ && source_id_ > 0)
    {
        navigation_system_->UnregisterSource(source_id_);
    }
    navigation_system_ = nullptr;
    source_id_ = 0;
    Component::OnDetach();
}

NavigationGeometry NavigationSourceComponent::GetGeometry() const
{
    NavigationGeometry geometry;
    return geometry;
}

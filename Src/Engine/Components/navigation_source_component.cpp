#include "navigation_source_component.h"
#include <cfloat>
#include "mesh_collider_component.h"
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
    NavigationGeometry geometry = {};
    if (!mesh_)
    {
        return geometry;
    }
    if (mesh_->GetVertices().empty())
    {
        return geometry;
    }

    geometry.indices = mesh_->GetIndices();
    geometry.vertices = mesh_->GetVertices();
    geometry.world_mat = mesh_->GetWorldMatrix();
    Box b;
    b.max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    b.min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);

    for (const auto& v : geometry.vertices)
    {
        const Vec3 w = TransformPoint(geometry.world_mat, v);
        if (b.max.x < w.x)
        {
            b.max.x = w.x;
        }
        if (b.max.y < w.y)
        {
            b.max.y = w.y;
        }
        if (b.max.z < w.z)
        {
            b.max.z = w.z;
        }
        if (b.min.x > w.x)
        {
            b.min.x = w.x;
        }
        if (b.min.y > w.y)
        {
            b.min.y = w.y;
        }
        if (b.min.z > w.z)
        {
            b.min.z = w.z;
        }
    }
    geometry.world_bounds = b;

    return geometry;
}

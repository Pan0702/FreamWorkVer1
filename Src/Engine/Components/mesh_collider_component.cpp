#include "mesh_collider_component.h"
#include "../../Debug/debug.h"
ColliderShape MeshColliderComponent::GetColliderShape() const
{
    return ColliderShape::kMesh;   
}

void MeshColliderComponent::OnAttach(const AttachContext& context)
{
    ColliderComponent::OnAttach(context);
    DEBUG_LOG("MeshCollider intersect is not implemented yet; treated asno-hit");
}

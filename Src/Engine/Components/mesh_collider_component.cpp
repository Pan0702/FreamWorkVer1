#include "mesh_collider_component.h"

#include "transform_component.h"
#include "../actor.h"
#include "../../Debug/debug.h"
#include "../../Resource/mesh.h"

MeshColliderComponent::MeshColliderComponent(const Mesh* mesh)
{
    if (mesh)
    {
        vertices_ = mesh->GetCollisionPositions();
        indices_  = mesh->GetCollisionIndices();
    }
}

void MeshColliderComponent::SetMesh(const Mesh* mesh)
{
    if (mesh)
    {
        vertices_ = mesh->GetCollisionPositions();
        indices_  = mesh->GetCollisionIndices();
    }  
}

ColliderShape MeshColliderComponent::GetColliderShape() const
{
    return ColliderShape::kMesh;   
}

void MeshColliderComponent::OnAttach(const AttachContext& context)
{
    ColliderComponent::OnAttach(context);
}

void MeshColliderComponent::SetTriangles(std::vector<Vec3> vertices, std::vector<uint32> indices)
{
    vertices_ = std::move(vertices);
    indices_  = std::move(indices);
}

const std::vector<Vec3>& MeshColliderComponent::GetVertices() const
{
    return vertices_; 
}

const std::vector<uint32>& MeshColliderComponent::GetIndices() const
{
    return indices_;  
}

Mat MeshColliderComponent::GetWorldMatrix() const
{
    Actor* actor = GetOwner();
    auto* t = actor ? actor->GetComponent<TransformComponent>() : nullptr;
    return t ? t->Matrix() : Identity();
}


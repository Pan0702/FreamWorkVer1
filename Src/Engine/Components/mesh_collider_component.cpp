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
    return actor ? actor->GetTransform().Matrix() : Identity();
}

void MeshColliderComponent::DrawDebug() const
{
    const Mat world = GetWorldMatrix();
    const auto& vertices = GetVertices();
    const auto& indices = GetIndices();
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        Vec3 a = TransformPoint(world, vertices[indices[i]]);
        Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        Debug::Get().DrawLine3D(a, b, color_);
        Debug::Get().DrawLine3D(b, c, color_);
        Debug::Get().DrawLine3D(c, a, color_);
    }
}

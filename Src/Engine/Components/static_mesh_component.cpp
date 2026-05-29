#include "static_mesh_component.h"

#include "../attach_context.h"
#include "../../Renderer/mesh_renderer.h"

StaticMeshComponent::StaticMeshComponent(Mesh* mesh, Material* material)
    : mesh_(mesh), material_(material)
{
}

StaticMeshComponent::~StaticMeshComponent()
{
    OnDetach();
}

void StaticMeshComponent::OnAttach(const AttachContext& context)
{
    renderer_ = context.mesh_renderer;
    if (renderer_ != nullptr)
    {
        renderer_->Register(this);
    }
}

void StaticMeshComponent::OnDetach()
{
    if (renderer_ != nullptr)
    {
        renderer_->Unregister(this);
        renderer_ = nullptr;
    }
}

void StaticMeshComponent::SetMesh(Mesh* mesh)
{
    mesh_ = mesh;
}

void StaticMeshComponent::SetMaterial(Material* material)
{
    material_ = material;
}

Mesh* StaticMeshComponent::GetMesh() const
{
    return mesh_;
}

Material* StaticMeshComponent::GetMaterial() const
{
    return material_;
}

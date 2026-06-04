#include "static_mesh_component.h"

#include "../attach_context.h"
#include "../../Renderer/mesh_renderer.h"


StaticMeshComponent::StaticMeshComponent(Mesh* mesh, MaterialSlot* material_slot)
    : mesh_(mesh), material_slot_(material_slot)
{
}

StaticMeshComponent::~StaticMeshComponent()
{
    StaticMeshComponent::OnDetach();
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


Mesh* StaticMeshComponent::GetMesh() const
{
    return mesh_;
}



MaterialSlot* StaticMeshComponent::GetMaterialSlot() const
{
    return material_slot_;
}

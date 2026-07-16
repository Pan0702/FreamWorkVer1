#include "instanced_static_mesh_component.h"
#include "../attach_context.h"
#include "../../Renderer/instanced_mesh_renderer.h"
InstancedStaticMeshComponent::InstancedStaticMeshComponent(Mesh* mesh, MaterialSlot* material)
{
    mesh_ = mesh;
    material_slot_ = material;
}

void InstancedStaticMeshComponent::OnAttach(const AttachContext& context)
{
    renderer_ = context.instanced_mesh_renderer;
    if (renderer_)
    {
        renderer_->Register(this);
    }
}

void InstancedStaticMeshComponent::OnDetach()
{
    if (renderer_)
    {
        renderer_->Unregister(this);
        renderer_ = nullptr;
    }
}

int32_t InstancedStaticMeshComponent::AddInstance(const Mat& transform)
{
    const InstancedID id = next_id_++;
    InstanceData instanced = {};
    instanced.id = id;
    instanced.local = transform;
    instances_.push_back(instanced);
    return id;
}

void InstancedStaticMeshComponent::SetInstanceTransform(uint32_t id, const Mat& transform)
{
    InstanceData* instance = FindInstance(id);
    if (instance == nullptr)
    {
        return;
    }
    instance->local = transform;
}

void InstancedStaticMeshComponent::RemoveInstance(uint32_t id)
{
auto it = std::ranges::find(instances_,id,&InstanceData::id);
    if (it != instances_.end())
    {
        instances_.erase(it);
    }
}

void InstancedStaticMeshComponent::ClearInstances()
{
    instances_.clear();
}

InstanceData* InstancedStaticMeshComponent::FindInstance(InstancedID id)
{
    auto it = std::ranges::find(instances_,id,&InstanceData::id);
    
    if (it == instances_.end())
    {
        return nullptr;
    }
    return &*it;
}

void InstancedStaticMeshComponent::SetMesh(Mesh* mesh)
{
    mesh_ = mesh;
}

void InstancedStaticMeshComponent::SetMaterialSlot(MaterialSlot* material)
{
    material_slot_ = material; 
}

Mesh* InstancedStaticMeshComponent::GetMesh() const
{
    return mesh_;
}

MaterialSlot* InstancedStaticMeshComponent::GetMaterialSlot() const
{
    return material_slot_;
}

const std::vector<InstanceData>& InstancedStaticMeshComponent::GetInstances() const
{
    return instances_;
}

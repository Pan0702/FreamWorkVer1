#include "mesh_renderer.h"

#include <algorithm>

#include "../Core/Math/my_math.h"
#include "../Engine/Components/static_mesh_component.h"
#include "../Engine/Components/transform_component.h"
#include "../Engine/actor.h"
#include "../Engine/camera.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Resource/material.h"
#include "../Resource/mesh.h"
#include "render_context.h"
#include "render_object.h"

bool MeshRenderer::Initialize(ID3D12Device* device)
{
    (void)device;
    return true;
}

void MeshRenderer::Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
                          Camera* camera, DescriptorHeap* descriptor_heap)
{
    for (auto& object : render_objects)
    {
        Mat wvp = Transpose(object->GetTransform() * camera->GetViewMatrix() * camera->GetProjectionMatrix());
        object->GetMaterial()->Apply(command_list, descriptor_heap);
        (void)wvp;

        D3D12_VERTEX_BUFFER_VIEW vbv = object->GetMesh()->GetVertexBufferView();
        command_list->IASetVertexBuffers(0, 1, &vbv);
        D3D12_INDEX_BUFFER_VIEW ibv = object->GetMesh()->GetIndexBufferView();
        command_list->IASetIndexBuffer(&ibv);
        command_list->DrawIndexedInstanced(object->GetMesh()->GetIndexCount(), 1, 0, 0, 0);
    }
}

void MeshRenderer::Register(StaticMeshComponent* component)
{
    if (component == nullptr)
    {
        return;
    }
    if (std::ranges::find(registered_, component) == registered_.end())
    {
        registered_.push_back(component);
    }
}

void MeshRenderer::Unregister(StaticMeshComponent* component)
{
    std::erase(registered_, component);
}

void MeshRenderer::Collect()
{
    draw_commands_.clear();
    for (StaticMeshComponent* component : registered_)
    {
        if (component == nullptr || component->GetMesh() == nullptr || component->GetMaterial() == nullptr)
        {
            continue;
        }

        Mat world = Identity();
        if (Actor* owner = component->GetOwner())
        {
            if (auto* transform = owner->GetComponent<TransformComponent>())
            {
                world = transform->Matrix();
            }
        }

        DrawCommand command = {};
        command.mesh = component->GetMesh();
        command.material = component->GetMaterial();
        command.world = world;
        draw_commands_.push_back(command);
    }
}

void MeshRenderer::Sort()
{
    std::ranges::sort(draw_commands_,
                      [](const DrawCommand& a, const DrawCommand& b)
                      {
                          return a.sort_key < b.sort_key;
                      });
}

void MeshRenderer::Submit(RenderContext& context)
{
    for (const DrawCommand& command : draw_commands_)
    {
        ConstantBufferAllocation allocation = {};
        Mat wvp = Transpose(command.world * context.view * context.projection);
        if (!context.cb_allocator->Allocate(sizeof(wvp), &allocation))
        {
            continue;
        }
        memcpy(allocation.cpu, &wvp, sizeof(wvp));

        command.material->Apply(context.command_list, context.srv_heap);
        context.command_list->SetGraphicsRootConstantBufferView(0, allocation.gpu);

        D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();
        context.command_list->IASetVertexBuffers(0, 1, &vbv);
        D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
        context.command_list->IASetIndexBuffer(&ibv);
        context.command_list->DrawIndexedInstanced(command.mesh->GetIndexCount(), 1, 0, 0, 0);
    }
}

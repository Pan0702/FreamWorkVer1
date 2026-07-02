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
#include "../Resource/material_slot.h"
#include "cb_file.h"

bool MeshRenderer::Initialize(const ID3D12Device* device)
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
        if (component == nullptr || component->GetMesh() == nullptr || component->GetMaterialSlot() == nullptr)
        {
            continue;
        }

        Mat world = Identity();
        if (Actor* owner = component->GetOwner())
        {
            world = owner->GetTransform().Matrix();
        }

        DrawCommand command = {};
        command.mesh = component->GetMesh();
        command.world = world;
        command.material_slot = component->GetMaterialSlot();
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

void MeshRenderer::Submit(RenderContext& context) const
{
    ConstantBufferAllocation light_alloc = {};
    CB::LightCB light_cb = {};
    light_cb.light_pos = Vec4(context.light_pos.x, context.light_pos.y, context.light_pos.z, 0.0f);
    light_cb.light_color = Vec4(context.light_color.x, context.light_color.y, context.light_color.z, 0.0f);
    light_cb.sky_color = Vec4(context.sky_color.x, context.sky_color.y, context.sky_color.z, 0.0f);
    light_cb.ground_color = Vec4(context.ground_color.x, context.ground_color.y, context.ground_color.z, 0.0f);
    light_cb.camera_pos = Vec4(context.camera_pos.x, context.camera_pos.y, context.camera_pos.z, 1.0f);
    const bool has_light = context.cb_allocator->Allocate(sizeof(light_cb), &light_alloc);
    if (has_light)
    {
        memcpy(light_alloc.cpu, &light_cb, sizeof(light_cb));
    }
    context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    for (const DrawCommand& command : draw_commands_)
    {
        // オブジェクトごとの行列を Constant Buffer に詰める。
        CB::MeshObjectCB obj = {};
        obj.world = Transpose(command.world);
        obj.wvp = Transpose(command.world * context.view * context.projection);
        ConstantBufferAllocation alloc = {};
        if (!context.cb_allocator->Allocate(sizeof(obj), &alloc))
        {
            continue;
        }
        memcpy(alloc.cpu, &obj, sizeof(obj));

        // 頂点とインデックスバッファをバインドして 1 サブメッシュを描画する。
        D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();
        context.command_list->IASetVertexBuffers(0, 1, &vbv);
        D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
        context.command_list->IASetIndexBuffer(&ibv);

        // サブメッシュ単位でマテリアルを反映する。
        for (const SubMesh& sub : command.mesh->GetSubMeshes())
        {
            Material* mat = command.material_slot->GetMaterial(sub.material_slot);
            if (mat == nullptr)
            {
                continue;
            }

            mat->Apply(context.command_list, context.srv_heap);

            // b0 には WVP と World 行列を設定する。
            context.command_list->SetGraphicsRootConstantBufferView(0, alloc.gpu);
            if (has_light)
            {
                context.command_list->SetGraphicsRootConstantBufferView(2, light_alloc.gpu);
            }

            // b2 にはサブメッシュのマテリアル定数を設定する。
            CB::MaterialCB mat_cb = {};
            mat_cb.base_color = mat->GetBaseColor();
            mat_cb.has_texture = (mat->GetDiffuse() != nullptr) ? 1 : 0;
            mat_cb.metallic = mat->GetMetallic();
            mat_cb.roughness = mat->GetRoughness();
            mat_cb.has_normal_map = (mat->GetNormal() != nullptr) ? 1 : 0;

            ConstantBufferAllocation mat_alloc = {};
            if (context.cb_allocator->Allocate(sizeof(mat_cb), &mat_alloc))
            {
                memcpy(mat_alloc.cpu, &mat_cb, sizeof(mat_cb));
                context.command_list->SetGraphicsRootConstantBufferView(3, mat_alloc.gpu);
            }

            // 現在のサブメッシュを描画する。
            context.command_list->DrawIndexedInstanced(sub.index_count, 1, sub.index_start, 0, 0);
        }
    }
}

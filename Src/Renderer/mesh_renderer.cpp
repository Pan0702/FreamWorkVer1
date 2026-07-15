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

#include "../Graphics/descriptor_heap.h"

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

void MeshRenderer::Collect(FrameSnap& write_snap)
{
    write_snap.mesh_commands.clear();
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

        MeshDrawCommand command = {};
        command.mesh = component->GetMesh();
        command.world = world;
        command.material_slot = component->GetMaterialSlot();
        command.sort_key = component->sort_key;
        const uint32 command_index = static_cast<uint32>(
            write_snap.mesh_commands.size());
        write_snap.mesh_commands.push_back(command);

        SceneDrawItem draw_item = {};
        draw_item.type = DrawType::kMesh;
        draw_item.draw_order = component->sort_key;
        draw_item.command_index = command_index;

        write_snap.draw_items.push_back(draw_item);
    }
}

void MeshRenderer::Submit(RenderContext& context, const MeshDrawCommand& command, const CameraSnap& cam)
{
    context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // オブジェクトごとの行列を Constant Buffer に詰める。
    CB::MeshObjectCB obj = {};
    obj.world = Transpose(command.world);
    obj.wvp = Transpose(command.world * cam.view * cam.projection);
    ConstantBufferAllocation alloc = {};
    if (!context.cb_allocator->Allocate(sizeof(obj), &alloc))
    {
        return;
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
        if (context.light_cb_address != 0)
        {
            context.command_list->SetGraphicsRootConstantBufferView(1, context.light_cb_address);
        }
        context.command_list->SetGraphicsRootDescriptorTable(
            5, context.srv_heap->GetGpuHandle(context.shadow_srv_index));
        context.command_list->SetGraphicsRootDescriptorTable(
            6, context.srv_heap->GetGpuHandle(context.irradiance_srv_index));
        // b2 にはサブメッシュのマテリアル定数を設定する。
        CB::MaterialCB mat_cb = {};
        mat_cb.base_color = mat->GetBaseColor();
        mat_cb.flag = mat->GetHasFlag();
        mat_cb.metallic = mat->GetMetallic();
        mat_cb.roughness = mat->GetRoughness();
        mat_cb.height_scale = mat->GetHeightScale();


        ConstantBufferAllocation mat_alloc = {};
        if (context.cb_allocator->Allocate(sizeof(mat_cb), &mat_alloc))
        {
            memcpy(mat_alloc.cpu, &mat_cb, sizeof(mat_cb));
            context.command_list->SetGraphicsRootConstantBufferView(2, mat_alloc.gpu);
        }

        // 現在のサブメッシュを描画する。
        context.command_list->DrawIndexedInstanced(sub.index_count, 1, sub.index_start, 0, 0);
    }
}


void MeshRenderer::SubmitDepth(RenderContext& context, const FrameSnap& read_snap)
{
    context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    for (const MeshDrawCommand& command : read_snap.mesh_commands)
    {
        CB::ShadowObjectCB obj = {};
        obj.wvp = Transpose(command.world * read_snap.light.lvp);
        ConstantBufferAllocation alloc = {};
        if (!context.cb_allocator->Allocate(sizeof(obj), &alloc))
        {
            continue;
        }
        memcpy(alloc.cpu, &obj, sizeof(obj));

        D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();
        context.command_list->IASetVertexBuffers(0, 1, &vbv);
        D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
        context.command_list->IASetIndexBuffer(&ibv);
        context.command_list->SetGraphicsRootConstantBufferView(0, alloc.gpu);
        context.command_list->DrawIndexedInstanced(command.mesh->GetIndexCount(), 1, 0, 0, 0);
    }
}

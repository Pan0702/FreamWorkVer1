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
#include "../Renderer/render_context.h"
#include "../Graphics/descriptor_heap.h"
#include "../Graphics/root_param.h"
#include "../Graphics/pipeline_state_cache.h"
#include "../Graphics/pipeline_state.h"

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

        // PSO と RootSignature はキャッシュから引く。シェーダは Material が決める。
        context.command_list->SetGraphicsRootSignature(
            context.pso_cache->GetRootSignature(VertexFactoryId::kStatic));
        context.command_list->SetPipelineState(
            context.pso_cache->Get(mat->GetShaderId(), VertexFactoryId::kStatic)->GetPipelineState());
        ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
        context.command_list->SetDescriptorHeaps(1, heaps);
        
        const MaterialBinding b = mat->GetBinding();
        
        BindMaterialTexture(context.command_list, context.srv_heap, b,
                                     ToIndex(StaticRootParam::kDiffuse), ToIndex(StaticRootParam::kNormal),
                                     ToIndex(StaticRootParam::kSpecular), ToIndex(StaticRootParam::kHeight));

        // b0 には WVP と World 行列を設定する。
        context.command_list->SetGraphicsRootConstantBufferView(ToIndex(StaticRootParam::kObjectCB), alloc.gpu);
        if (context.light_cb_address != 0)
        {
            context.command_list->SetGraphicsRootConstantBufferView(ToIndex(StaticRootParam::kLightCB),
                                                                    context.light_cb_address);
        }
        context.command_list->SetGraphicsRootDescriptorTable(ToIndex(StaticRootParam::kShadow)
                                                             , context.srv_heap->GetGpuHandle(
                                                                 context.shadow_srv_index));
        context.command_list->SetGraphicsRootDescriptorTable(ToIndex(StaticRootParam::kIrradiance)
                                                             , context.srv_heap->GetGpuHandle(
                                                                 context.irradiance_srv_index));


        ConstantBufferAllocation mat_alloc = {};
        if (context.cb_allocator->Allocate(sizeof(b.cb), &mat_alloc))
        {
            memcpy(mat_alloc.cpu, &b.cb, sizeof(b.cb));
            context.command_list->SetGraphicsRootConstantBufferView(ToIndex(StaticRootParam::kMaterialCB),
                                                                    mat_alloc.gpu);
        }

        // 現在のサブメッシュを描画する。
        context.command_list->DrawIndexedInstanced(sub.index_count, 1,
                                                   sub.index_start, 0, 0);
    }
}


void MeshRenderer::SubmitDepth(const RenderContext& context, const FrameSnap& read_snap) const
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
        context.command_list->SetGraphicsRootConstantBufferView(ToIndex(ShadowRootParam::kObjectCB)
                                                                , alloc.gpu);
        context.command_list->DrawIndexedInstanced(command.mesh->GetIndexCount(), 1,
                                                   0, 0, 0);
    }
}

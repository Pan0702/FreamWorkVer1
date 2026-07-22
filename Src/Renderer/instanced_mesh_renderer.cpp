#include "instanced_mesh_renderer.h"

#include "cb_file.h"
#include "render_context.h"
#include "../Engine/actor.h"
#include "../Engine/Components/instanced_static_mesh_component.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Graphics/descriptor_heap.h"
#include "../Graphics/pipeline_state.h"

bool InstancedMeshRenderer::Initialize(const ID3D12Device* device)
{
    (void)device;
    return true;
}

void InstancedMeshRenderer::Register(InstancedStaticMeshComponent* object)
{
    if (object == nullptr)
    {
        return;
    }

    if (std::ranges::find(registered_, object) == registered_.end())
    {
        registered_.push_back(object);
    }
}

void InstancedMeshRenderer::Unregister(InstancedStaticMeshComponent* object)
{
    std::erase(registered_, object);
}

void InstancedMeshRenderer::Collect(FrameSnap& write_snap) const
{
    write_snap.gpu_instances.clear();
    write_snap.instanced_mesh_commands.clear();

    for (InstancedStaticMeshComponent* component : registered_)
    {
        if (component == nullptr || component->GetMesh() == nullptr || component->GetMaterialSlot() == nullptr)
        {
            continue;
        }

        Mat owner_world = Identity();
        if (Actor* owner = component->GetOwner())
        {
            owner_world = owner->GetTransform().Matrix();
        }
        const uint32 instance_offset = static_cast<uint32>(write_snap.gpu_instances.size());
        for (const InstanceData& instance : component->GetInstances())
        {
            GPUInstanceData gpu = {};
            gpu.world = instance.local * owner_world;
            write_snap.gpu_instances.push_back(gpu);
        }

        InstancedMeshDrawCommand command = {};
        command.mesh = component->GetMesh();
        command.material_slot = component->GetMaterialSlot();
        command.instance_offset = instance_offset;
        command.instance_count = static_cast<uint32>(component->GetInstances().size());

        const uint32 command_index = static_cast<uint32>(write_snap.instanced_mesh_commands.size());
        write_snap.instanced_mesh_commands.push_back(command);

        SceneDrawItem item = {};
        item.type = DrawType::kInstancedMesh;
        item.draw_order = component->sort_key;
        item.command_index = command_index;

        write_snap.draw_items.push_back(item);
    }
}

void InstancedMeshRenderer::Submit(const RenderContext& context, const InstancedMeshDrawCommand& command,
                                   const FrameSnap& read_snap)
{
    auto* command_list = context.command_list;
    if (command_list == nullptr || context.cb_allocator == nullptr ||
        command.mesh == nullptr || command.material_slot == nullptr || command.instance_count == 0)
    {
        return;
    }

    const size_t instance_offset = command.instance_offset;
    const size_t instance_count = command.instance_count;

    if (instance_offset > read_snap.gpu_instances.size() ||
        instance_count > read_snap.gpu_instances.size() - instance_offset)
    {
        return;
    }

    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //Upload領域の確保
    const uint32 buffer_size = sizeof(GPUInstanceData) * command.instance_count;
    ConstantBufferAllocation allocation = {};
    if (!context.cb_allocator->Allocate(buffer_size, &allocation))
    {
        return;
    }

    auto* destination = static_cast<GPUInstanceData*>(allocation.cpu);
    for (uint32 i = 0; i < command.instance_count; ++i)
    {
        const GPUInstanceData& src = read_snap.gpu_instances[instance_offset + i];
        destination[i].world = Transpose(src.world);
    }

    const D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();

    command_list->IASetVertexBuffers(0, 1, &vbv);

    const D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
    command_list->IASetIndexBuffer(&ibv);

    CB::InstacedCB cb = {};
    cb.vp = Transpose(read_snap.camera.view * read_snap.camera.projection);
    ConstantBufferAllocation cb_alloc = {};
    if (!context.cb_allocator->Allocate(sizeof(cb), &cb_alloc))
    {
        return;
    }
    memcpy(cb_alloc.cpu, &cb, sizeof(cb));
    for (const SubMesh& sub : command.mesh->GetSubMeshes())
    {
        Material* mat = command.material_slot->GetMaterial(sub.material_slot);
        if (mat == nullptr)
        {
            continue;
        }

        command_list->SetGraphicsRootSignature(
            context.pso_cache->GetRootSignature(VertexFactoryId::kInstanced));
        command_list->SetPipelineState(
            context.pso_cache->Get(mat->GetShaderId(), VertexFactoryId::kInstanced)->GetPipelineState());
        ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
        command_list->SetDescriptorHeaps(1, heaps);

        // テクスチャ SRV をバインド。
        const MaterialBinding b = mat->GetBinding();
        BindMaterialTexture(command_list, context.srv_heap, b,
                             ToIndex(StaticRootParam::kDiffuse), ToIndex(StaticRootParam::kNormal),
                             ToIndex(StaticRootParam::kSpecular), ToIndex(StaticRootParam::kHeight));

        //b0 
        command_list->SetGraphicsRootConstantBufferView(ToIndex(StaticRootParam::kObjectCB), cb_alloc.gpu);

        //t6 InstanceData
        command_list->SetGraphicsRootShaderResourceView(ToIndex(StaticRootParam::kInstanceMatrics), allocation.gpu);

        //b1 light
        if (context.light_cb_address != 0)
        {
            command_list->SetGraphicsRootConstantBufferView(ToIndex(StaticRootParam::kLightCB), context.light_cb_address);
        }

        ConstantBufferAllocation mat_alloc = {};
        if (!context.cb_allocator->Allocate(sizeof(b.cb), &mat_alloc))
        {
            return;
        }
        memcpy(mat_alloc.cpu, &b.cb, sizeof(b.cb));
        context.command_list->SetGraphicsRootConstantBufferView(
            ToIndex(StaticRootParam::kMaterialCB), mat_alloc.gpu);
        //ShadowMap
        command_list->SetGraphicsRootDescriptorTable(
            ToIndex(StaticRootParam::kShadow), context.srv_heap->GetGpuHandle(context.shadow_srv_index));

        // Irradiance Map
        command_list->SetGraphicsRootDescriptorTable(ToIndex(StaticRootParam::kIrradiance), context.srv_heap->GetGpuHandle(
                                                                 context.irradiance_srv_index));

        command_list->DrawIndexedInstanced(sub.index_count, command.instance_count,
                                                   sub.index_start, 0, 0);
    }
}

void InstancedMeshRenderer::SubmitDepth(const RenderContext& context, const FrameSnap& read_snap) const
{
    for (const InstancedMeshDrawCommand& command : read_snap.instanced_mesh_commands)
    {
        if (command.mesh == nullptr || command.instance_count == 0)
        {
            continue;
        }
        const size_t instance_offset = command.instance_offset;
        const size_t instance_count = command.instance_count;

        if (instance_offset > read_snap.gpu_instances.size() ||
            instance_count > read_snap.gpu_instances.size() - instance_offset)
        {
            continue;
        }

        context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //Upload領域の確保
        const uint32 buffer_size = sizeof(GPUInstanceData) * command.instance_count;
        ConstantBufferAllocation allocation = {};
        if (!context.cb_allocator->Allocate(buffer_size, &allocation))
        {
            continue;
        }

        auto* destination = static_cast<GPUInstanceData*>(allocation.cpu);
        for (uint32 i = 0; i < command.instance_count; ++i)
        {
            const GPUInstanceData& src = read_snap.gpu_instances[instance_offset + i];
            destination[i].world = Transpose(src.world);
        }

        const D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();

        context.command_list->IASetVertexBuffers(0, 1, &vbv);

        const D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
        context.command_list->IASetIndexBuffer(&ibv);

        CB::InstacedCB cb = {};
        cb.vp = Transpose(read_snap.light.lvp);
        ConstantBufferAllocation cb_alloc = {};
        if (!context.cb_allocator->Allocate(sizeof(cb), &cb_alloc))
        {
            return;
        }
        memcpy(cb_alloc.cpu, &cb, sizeof(cb));

        auto* command_list = context.command_list;
        // 全インスタンス共通のライトVP
        // Root Parameter 0
        command_list->SetGraphicsRootConstantBufferView(ToIndex(ShadowRootParam::kObjectCB), cb_alloc.gpu);

        // Root Parameter 1
        command_list->SetGraphicsRootShaderResourceView (ToIndex(ShadowRootParam::kInstanceMatrices), allocation.gpu);

        command_list->DrawIndexedInstanced(command.mesh->GetIndexCount(),
                                           command.instance_count, 0,
                                           0, 0);
    }
}

#include "skinned_mesh_renderer.h"

#include <algorithm>

#include "cb_file.h"
#include "render_context.h"
#include "../Engine/actor.h"
#include "../Engine/Components/skeletal_mesh.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Resource/skeltal_mesh.h"
#include "../Resource/vertex_types.h"
#include  "../Engine/Components/transform_component.h"
#include "../Graphics/descriptor_heap.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Resource/material_slot.h"
#include "../Resource/texture2D.h"
#include "../Engine/Components/animation_component.h"

bool SkinnedMeshRenderer::Initialize(ID3D12Device* device)
{
    root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder builder;
    builder
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddCbv(1, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddCbv(1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddCbv(2, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(1, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(2, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(3, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(4, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(5, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP)
        .AddComparisonSampler(1, D3D12_SHADER_VISIBILITY_PIXEL);

    if (!builder.Build(device, root_signature_.get()))
    {
        return false;
    }
    auto vs = std::make_unique<Shader>();
    if (!vs->LoadFromFile(L"Shaders/skinned.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }
    auto ps = std::make_unique<Shader>();
    if (!ps->LoadFromFile(L"Shaders/triangle.ps.hlsl", "PSMain", "ps_5_0"))
    {
        return false;
    }

    pipeline_state_ = std::make_unique<PipelineState>();
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = kSkinnedVertexLayout;;
    layout_desc.NumElements = _countof(kSkinnedVertexLayout);
    PipelineStateBuilder ps_builder;
    ps_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vs->GetBytecode())
        .SetPixelShader(ps->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetDepthEnabled(true)
        .SetAlphaBlendEnabled(false);
    if (!ps_builder.Build(device, pipeline_state_.get()))
    {
        return false;
    }

    return true;
}

void SkinnedMeshRenderer::Render(ID3D12GraphicsCommandList* command_list,
                                 const std::vector<RenderObject*>& render_objects, Camera* camera,
                                 DescriptorHeap* descriptor_heap)
{
}

void SkinnedMeshRenderer::Register(SkeletalMeshComponent* component)
{
    if (component && std::ranges::find(meshes_, component) == meshes_.end())
    {
        meshes_.push_back(component);
    }
}

void SkinnedMeshRenderer::Unregister(SkeletalMeshComponent* component)
{
    std::erase(meshes_, component);
}

void SkinnedMeshRenderer::Collect(FrameSnap& write_snap)
{
    write_snap.skinned_commands.clear();
    for (SkeletalMeshComponent* component : meshes_)
    {
        if (!component || !component->GetSkeletalMesh() || !component->GetMaterialSlot())
        {
            continue;
        }
        Mat world = Identity();
        std::vector<Mat> bone_palette;
        if (Actor* owner = component->GetOwner())
        {
            world = owner->GetTransform().Matrix();
            if (auto* s = owner->GetComponent<AnimationComponent>())
            {
                bone_palette = s->GetBonePalette();
            }
        }
        SkinnedDrawCommand command = {};
        command.mesh = component->GetSkeletalMesh();
        command.material_slot = component->GetMaterialSlot();
        command.sort_key = component->sort_key;
        command.world = world;
        command.bone_palette = bone_palette;
        const uint32 command_index = static_cast<uint32>(
            write_snap.skinned_commands.size());
        write_snap.skinned_commands.push_back(command);

        SceneDrawItem item = {};
        item.type = DrawType::kSkinnedMesh;
        item.draw_order = component->sort_key;
        item.command_index = command_index;

        write_snap.draw_items.push_back(item);
    }
}

void SkinnedMeshRenderer::Submit(RenderContext& context, const SkinnedDrawCommand& command,
                                 const CameraSnap& cam) const
{
    auto command_list = context.command_list;
    command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
    command_list->SetDescriptorHeaps(1, heaps);
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    auto cb_allocator = context.cb_allocator;


    //b0
    CB::MeshObjectCB obj = {};
    obj.world = Transpose(command.world);
    obj.wvp = Transpose(command.world * cam.view * cam.projection);
    ConstantBufferAllocation obj_alloc = {};
    if (!cb_allocator->Allocate(sizeof(obj), &obj_alloc))
    {
        return;
    }
    memcpy(obj_alloc.cpu, &obj, sizeof(obj));

    //b1 
    CB::BoneCB bone = {};
    for (int i = 0; i < kMaxBones; ++i)
    {
        bone.bones[i] = Identity();
    }
    if (!command.bone_palette.empty())
    {
        const int bone_count = static_cast<int>(std::min(command.bone_palette.size(),
                                                         static_cast<size_t>(kMaxBones)));
        for (int i = 0; i < bone_count; ++i)
        {
            bone.bones[i] = Transpose((command.bone_palette)[static_cast<size_t>(i)]);
        }
    }
    ConstantBufferAllocation bone_alloc = {};
    if (!cb_allocator->Allocate(sizeof(bone), &bone_alloc))
    {
        return;
    }
    memcpy(bone_alloc.cpu, &bone, sizeof(bone));
    // b0
    command_list->SetGraphicsRootConstantBufferView(0, obj_alloc.gpu);
    // b1 vs
    command_list->SetGraphicsRootConstantBufferView(1, bone_alloc.gpu);
    if (context.light_cb_address != 0)
    {
        //b1 ps
        command_list->SetGraphicsRootConstantBufferView(2, context.light_cb_address);
    }
    D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();
    command_list->IASetVertexBuffers(0, 1, &vbv);
    D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
    command_list->IASetIndexBuffer(&ibv);

    for (const SubMesh& sub : command.mesh->GetSubMeshes())
    {
        Material* mat = command.material_slot->GetMaterial(sub.material_slot);
        if (!mat)
        {
            continue;
        }
        const uint32 diff = (mat->GetDiffuse() ? mat->GetDiffuse()->GetSrvIndex() : 0);
        const uint32 norm = (mat->GetNormal() ? mat->GetNormal()->GetSrvIndex() : 0);
        const uint32 specular = (mat->GetSpecular() ? mat->GetSpecular()->GetSrvIndex() : 0);
        const uint32 height = (mat->GetHeight() ? mat->GetHeight()->GetSrvIndex() : 0);

        command_list->SetGraphicsRootDescriptorTable(4, context.srv_heap->GetGpuHandle(diff));
        command_list->SetGraphicsRootDescriptorTable(5, context.srv_heap->GetGpuHandle(norm));
        command_list->SetGraphicsRootDescriptorTable(6, context.srv_heap->GetGpuHandle(context.shadow_srv_index));
        command_list->SetGraphicsRootDescriptorTable(
            7, context.srv_heap->GetGpuHandle(context.irradiance_srv_index));
        command_list->SetGraphicsRootDescriptorTable(8, context.srv_heap->GetGpuHandle(specular));
        command_list->SetGraphicsRootDescriptorTable(9, context.srv_heap->GetGpuHandle(height));

        // b2
        CB::MaterialCB mat_cb = {};
        mat_cb.base_color = mat->GetBaseColor();
        mat_cb.metallic = mat->GetMetallic();
        mat_cb.roughness = mat->GetRoughness();
        mat_cb.flag = mat->GetHasFlag();
        mat_cb.height_scale = mat->GetHeightScale();

        ConstantBufferAllocation mat_alloc = {};
        if (cb_allocator->Allocate(sizeof(mat_cb), &mat_alloc))
        {
            memcpy(mat_alloc.cpu, &mat_cb, sizeof(mat_cb));
            command_list->SetGraphicsRootConstantBufferView(3, mat_alloc.gpu);
            command_list->DrawIndexedInstanced(sub.index_count, 1, sub.index_start, 0, 0);
        }
    }
}

void SkinnedMeshRenderer::SubmitDepth(RenderContext& context, const FrameSnap& read_snap) const
{
    if (read_snap.skinned_commands.empty())
    {
        return;
    }
    auto command_list = context.command_list;
    // シャドウパスでは通常メッシュを三角形リストとして描画し、色は出さず深度だけを書き込む。
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (const SkinnedDrawCommand& command : read_snap.skinned_commands)
    {
        CB::ShadowObjectCB obj = {};
        obj.wvp = Transpose(command.world * read_snap.light.lvp);

        // オブジェクトごとのライト視点 WVP を、フレーム用定数バッファへ書き込む。
        ConstantBufferAllocation alloc = {};
        if (!context.cb_allocator->Allocate(sizeof(obj), &alloc))
        {
            continue;
        }
        memcpy(alloc.cpu, &obj, sizeof(obj));

        CB::BoneCB bone = {};
        for (int i = 0; i < kMaxBones; ++i)
        {
            bone.bones[i] = Identity();
        }
        if (!command.bone_palette.empty())
        {
            const int bone_count = static_cast<int>(std::min(command.bone_palette.size(),
                                                             static_cast<size_t>(kMaxBones)));
            for (int i = 0; i < bone_count; ++i)
            {
                bone.bones[i] = Transpose((command.bone_palette)[static_cast<size_t>(i)]);
            }
        }
        ConstantBufferAllocation bone_alloc = {};
        if (!context.cb_allocator->Allocate(sizeof(bone), &bone_alloc))
        {
            continue;
        }
        memcpy(bone_alloc.cpu, &bone, sizeof(bone));

        command_list->SetGraphicsRootConstantBufferView(0, alloc.gpu);
        command_list->SetGraphicsRootConstantBufferView(1, bone_alloc.gpu);

        // 通常メッシュの頂点・インデックスを使い、シャドウ用 PSO で深度だけを描画する。
        D3D12_VERTEX_BUFFER_VIEW vbv = command.mesh->GetVertexBufferView();
        command_list->IASetVertexBuffers(0, 1, &vbv);
        D3D12_INDEX_BUFFER_VIEW ibv = command.mesh->GetIndexBufferView();
        command_list->IASetIndexBuffer(&ibv);
        command_list->DrawIndexedInstanced(command.mesh->GetIndexCount(), 1, 0, 0, 0);
    }
}

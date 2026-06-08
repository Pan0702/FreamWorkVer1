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
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddCbv(1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddCbv(2, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(1, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP);

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

void SkinnedMeshRenderer::Collect()
{
    draw_commands_.clear();
    for (SkeletalMeshComponent* component : meshes_)
    {
        if (!component || !component->GetSkeltalMesh() || !component->GetMaterialSlot())
        {
            continue;
        }
        Mat world = Identity();
        const std::vector<Mat>* bone_palette = nullptr;
        if (Actor* owner = component->GetOwner())
        {
            if (auto* t = owner->GetComponent<TransformComponent>())
            {
                world = t->Matrix();
            }
            if (auto* s = owner->GetComponent<AnimationComponent>())
            {
                bone_palette = &s->GetBonePalette();
            }
        }
        SkinnedDrawCommand command = {};
        command.mesh = component->GetSkeltalMesh();
        command.material_slot = component->GetMaterialSlot();
        command.world = world;
        command.bone_palette = bone_palette;
        draw_commands_.push_back(command);
    }
}

void SkinnedMeshRenderer::Sort()
{
    std::ranges::sort(draw_commands_,
                      [](const SkinnedDrawCommand& a, const SkinnedDrawCommand& b)
                      {
                          return a.material_slot < b.material_slot;
                      });
}

void SkinnedMeshRenderer::Submit(RenderContext& context)
{
    if (draw_commands_.empty())
    {
        return;
    }
    auto command_list = context.command_list;
    command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
    command_list->SetDescriptorHeaps(1, heaps);
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    auto cb_allocator = context.cb_allocator;
    //Light
    CB::LightCB light = {};
    light.light_dir = Vec4(context.light_dir.x, context.light_dir.y, context.light_dir.z, 0.0f);
    light.light_color = Vec4(context.light_color.x, context.light_color.y, context.light_color.z, 0.0f);
    light.ambient = Vec4(context.ambient.x, context.ambient.y, context.ambient.z, 0.0f);
    light.camera_pos = Vec4(context.camera_pos.x, context.camera_pos.y, context.camera_pos.z, 1.0f);
    ConstantBufferAllocation light_alloc = {};
    bool has_light = cb_allocator->Allocate(sizeof(light), &light_alloc);
    if (has_light)
    {
        memcpy(light_alloc.cpu, &light, sizeof(light));
    }

    for (const SkinnedDrawCommand& command : draw_commands_)
    {
        //b0
        CB::MeshObjectCB obj = {};
        obj.world = Transpose(command.world);
        obj.wvp = Transpose(command.world * context.view * context.projection);
        ConstantBufferAllocation obj_alloc = {};
        if (!cb_allocator->Allocate(sizeof(obj), &obj_alloc))
        {
            continue;
        }
        memcpy(obj_alloc.cpu, &obj, sizeof(obj));

        //b1 
        CB::BoneCB bone = {};
        for (int i = 0; i < kMaxBones; ++i)
        {
            bone.bones[i] = Identity();
        }
        if (command.bone_palette != nullptr)
        {
            const int bone_count = static_cast<int>(std::min(command.bone_palette->size(),
                                                             static_cast<size_t>(kMaxBones)));
            for (int i = 0; i < bone_count; ++i)
            {
                bone.bones[i] = Transpose((*command.bone_palette)[static_cast<size_t>(i)]);
            }
        }
        ConstantBufferAllocation bone_alloc = {};
        if (!cb_allocator->Allocate(sizeof(bone), &bone_alloc))
        {
            continue;
        }
        memcpy(bone_alloc.cpu, &bone, sizeof(bone));
        // b0
        command_list->SetGraphicsRootConstantBufferView(0, obj_alloc.gpu);
        // b1 vs
        command_list->SetGraphicsRootConstantBufferView(1, bone_alloc.gpu);
        if (has_light)
        {
            //b1 ps
            command_list->SetGraphicsRootConstantBufferView(3, light_alloc.gpu);
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
            uint32 diff = (mat->GetDiffuse() ? mat->GetDiffuse()->GetSrvIndex() : 0);
            uint32 norm = (mat->GetNormal() ? mat->GetNormal()->GetSrvIndex() : 0);
            command_list->SetGraphicsRootDescriptorTable(2, context.srv_heap->GetGpuHandle(diff));
            command_list->SetGraphicsRootDescriptorTable(5, context.srv_heap->GetGpuHandle(norm));

            // b2
            CB::MaterialCB mat_cb = {};
            mat_cb.base_color = mat->GetBaseColor();
            mat_cb.has_texture = (mat->GetDiffuse() != nullptr ? 1 : 0);
            mat_cb.specular_intensity = mat->GetSpecularIntensity();
            mat_cb.specular_power = mat->GetSpecularPower();
            mat_cb.has_normal_map = (mat->GetNormal() != nullptr ? 1 : 0);
            ConstantBufferAllocation mat_alloc = {};
            if (cb_allocator->Allocate(sizeof(mat_cb), &mat_alloc))
            {
                memcpy(mat_alloc.cpu, &mat_cb, sizeof(mat_cb));
                command_list->SetGraphicsRootConstantBufferView(4, mat_alloc.gpu);
                command_list->DrawIndexedInstanced(sub.index_count, 1, sub.index_start, 0, 0);
            }
        }
    }
}

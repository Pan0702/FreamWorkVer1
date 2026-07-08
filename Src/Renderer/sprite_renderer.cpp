#include "sprite_renderer.h"

#include <algorithm>

#include "../Engine/Components/sprite_component.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Graphics/descriptor_heap.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Graphics/vertex_buffer.h"
#include "../Resource/texture2D.h"
#include "../Resource/vertex_types.h"
#include "render_context.h"

bool SpriteRenderer::Initialize(ID3D12Device* device)
{
    SpriteVertex vertices[4] = {
        {{-0.5f, -0.5f}, {0.0f, 0.0f}},
        {{+0.5f, -0.5f}, {1.0f, 0.0f}},
        {{+0.5f, +0.5f}, {1.0f, 1.0f}},
        {{-0.5f, +0.5f}, {0.0f, 1.0f}},
    };
    uint16_t indices[6] = {0, 1, 2, 2, 3, 0};

    quad_vb_ = std::make_unique<VertexBuffer>();
    if (!quad_vb_->Initialize(device, vertices, sizeof(vertices), sizeof(vertices[0])))
    {
        return false;
    }

    quad_ib_ = std::make_unique<IndexBuffer>();
    if (!quad_ib_->Initialize(device, indices, sizeof(indices), DXGI_FORMAT_R16_UINT))
    {
        return false;
    }

    vertex_shader_ = std::make_unique<Shader>();
    if (!vertex_shader_->LoadFromFile(L"Shaders/sprite_world.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }

    pixel_shader_ = std::make_unique<Shader>();
    if (!pixel_shader_->LoadFromFile(L"Shaders/sprite_world.ps.hlsl", "PSMain", "ps_5_0"))
    {
        return false;
    }

    root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder rs_builder;
    rs_builder
        .AddCbv(0, D3D12_SHADER_VISIBILITY_ALL)
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    if (!rs_builder.Build(device, root_signature_.get()))
    {
        return false;
    }

    pipeline_state_ = std::make_unique<PipelineState>();
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = kSpriteVertexLayout;
    layout_desc.NumElements = _countof(kSpriteVertexLayout);

    PipelineStateBuilder ps_builder;
    ps_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vertex_shader_->GetBytecode())
        .SetPixelShader(pixel_shader_->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetDepthEnabled(true)
        .SetDepthWriteEnabled(false)
        .SetAlphaBlendEnabled(true);

    return ps_builder.Build(device, pipeline_state_.get());
}

void SpriteRenderer::Shutdown()
{
    registered_.clear();
    immediate_commands_.clear();
}

void SpriteRenderer::Register(SpriteComponent* component)
{
    if (component != nullptr && std::ranges::find(registered_, component) == registered_.end())
    {
        registered_.push_back(component);
    }
}

void SpriteRenderer::Unregister(SpriteComponent* component)
{
    std::erase(registered_, component);
}

void SpriteRenderer::DrawImmediate(const SpriteDrawCommand& command)
{
    immediate_commands_.push_back(command);
}

void SpriteRenderer::Collect(FrameSnap& write_snap)
{
    write_snap.sprite_commands.clear();
    for (SpriteComponent* component : registered_)
    {
        if (component == nullptr || component->texture == nullptr)
        {
            continue;
        }

        SpriteDrawCommand command = {};
        command.texture = component->texture;
        command.position = component->position;
        command.world = Translate(Vec3(component->position.x, component->position.y, 0.0f));
        command.size = component->size_;
        command.color = component->color;
        command.src_rect = component->src_rect;
        command.use_texture = component->texture != nullptr;
        write_snap.sprite_commands.push_back(command);
    }

    write_snap.sprite_commands.insert(write_snap.sprite_commands.end(), immediate_commands_.begin(),
                                      immediate_commands_.end());
    immediate_commands_.clear();
    std::ranges::sort(write_snap.sprite_commands,
                      [](const SpriteDrawCommand& a, const SpriteDrawCommand& b)
                      {
                          return a.sort_key < b.sort_key;
                      });
}

void SpriteRenderer::Submit(RenderContext& context, const FrameSnap& read_snap)
{
    context.command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    context.command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D12_VERTEX_BUFFER_VIEW vbv = quad_vb_->GetVertexBufferView();
    context.command_list->IASetVertexBuffers(0, 1, &vbv);
    D3D12_INDEX_BUFFER_VIEW ibv = quad_ib_->GetIndexBufferView();
    context.command_list->IASetIndexBuffer(&ibv);

    ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
    context.command_list->SetDescriptorHeaps(1, heaps);

    for (const SpriteDrawCommand& command : read_snap.sprite_commands)
    {
        SubmitCommand(context, command, read_snap);
    }
}

void SpriteRenderer::SubmitCommand(RenderContext& context, const SpriteDrawCommand& command,const FrameSnap& read_snap)
{
    SpriteWorldCBData cb_data = {};
    const Mat world = Scale(Vec3(command.size.x, command.size.y, 1.0f)) * command.world;
    cb_data.wvp = Transpose(read_snap.camera.view * read_snap.camera.projection);
    cb_data.color = command.color;
    cb_data.src_rect = command.src_rect;
    cb_data.options = Vec4(command.use_texture ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f);


    ConstantBufferAllocation allocation = {};
    if (!context.cb_allocator->Allocate(sizeof(cb_data), &allocation))
    {
        return;
    }
    memcpy(allocation.cpu, &cb_data, sizeof(cb_data));

    context.command_list->SetGraphicsRootConstantBufferView(0, allocation.gpu);
    const uint32_t srv_index = command.texture != nullptr ? command.texture->GetSrvIndex() : 0;
    context.command_list->SetGraphicsRootDescriptorTable(1, context.srv_heap->GetGpuHandle(srv_index));
    context.command_list->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

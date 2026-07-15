#include "ui_renderer.h"

#include <algorithm>

#include "../Resource/vertex_types.h"
#include "../Engine/Components/sprite_component.h"
#include "render_context.h"
#include "../Game/GameMain.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Graphics/descriptor_heap.h"
#include "../Resource/texture2D.h"

bool UIRenderer::Initialize(ID3D12Device* device)
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
    if (!vertex_shader_->LoadFromFile(L"Shaders/sprite.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }

    pixel_shader_ = std::make_unique<Shader>();
    if (!pixel_shader_->LoadFromFile(L"Shaders/sprite.ps.hlsl", "PSMain", "ps_5_0"))
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
        .SetDepthEnabled(false)
        .SetAlphaBlendEnabled(true);

    return ps_builder.Build(device, pipeline_state_.get());
}

void UIRenderer::Shutdown()
{
    registered_.clear();
    immediate_commands_.clear();
}

void UIRenderer::Register(SpriteComponent* component)
{
    if (component != nullptr && std::ranges::find(registered_, component) == registered_.end())
    {
        registered_.push_back(component);
    }
}

void UIRenderer::Unregister(SpriteComponent* component)
{
    std::erase(registered_, component);
}

void UIRenderer::DrawImmediate(const SpriteDrawCommand& command)
{
    immediate_commands_.push_back(command);
}

void UIRenderer::Collect(FrameSnap& write_snap)
{
    write_snap.ui_commands.clear();
    for (SpriteComponent* component : registered_)
    {
        if (component == nullptr || !component->GetVisible())
        {
            continue;
        }

        Vec2 draw_size = component->size_;
        Vec2 draw_pos = component->position;
        if (component->GetRelativeSizeFlag())
        {
            draw_size.x = component->GetSize().x * game_main->GetWindow().GetSize().width;
            draw_size.y = component->GetSize().y * game_main->GetWindow().GetSize().height;
        }
        if (component->GetRelativePosFlag())
        {
            draw_pos.x = component->GetPos().x * game_main->GetWindow().GetSize().width;
            draw_pos.y = component->GetPos().y * game_main->GetWindow().GetSize().height;
        }
        SpriteDrawCommand command = {};
        command.texture = component->texture;
        command.size = draw_size;
        command.position = draw_pos + draw_size * kHalfSize;
        command.color = component->color;
        command.src_rect = component->src_rect;
        command.use_texture = component->texture != nullptr;
        command.sort_key = component->sort_key;
        write_snap.ui_commands.push_back(command);
    }

    write_snap.ui_commands.insert(write_snap.ui_commands.end(), immediate_commands_.begin(), immediate_commands_.end());
    immediate_commands_.clear();
    
    std::ranges::sort(write_snap.ui_commands,
                  [](const SpriteDrawCommand& a, const SpriteDrawCommand& b)
                  {
                      return a.sort_key < b.sort_key;
                  });
}

void UIRenderer::Submit(const RenderContext& context, const std::vector<SpriteDrawCommand>& read_commands)
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

    for (const SpriteDrawCommand& command : read_commands)
    {
        SubmitCommand(context, command);
    }
}

void UIRenderer::SubmitCommand(const RenderContext& context, const SpriteDrawCommand& command)
{
    UISpriteCBData cb_data = {};
    cb_data.sprite_pos = command.position;
    cb_data.sprite_size = command.size;
    cb_data.color = command.color;
    cb_data.screen_size = context.screen_size;
    cb_data.rotation = command.rotation;
    cb_data.use_texture = command.use_texture ? 1.0f : 0.0f;

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

#include "sprite_renderer.h"
#include "../Graphics/root_signature.h"
#include "../Resource/vertex_types.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/vertex_buffer.h"
#include "../Graphics/shader.h"
#include "../Graphics/constant_buffer.h"


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
    if (!quad_vb_->Initialize(device, vertices,sizeof(vertices), sizeof(vertices[0])))
    {
        return false;
    }

    quad_ib_ = std::make_unique<IndexBuffer>();
    if (!quad_ib_->Initialize(device, indices,sizeof(indices), DXGI_FORMAT_R16_UINT))
    {
        return false;
    }
    
    vertex_shader_ = std::make_unique<Shader>();
    if (!vertex_shader_->LoadFromFile(L"Shaders/sprite.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }

    pixel_shader_ = std::make_unique<Shader>();
    if(!pixel_shader_->LoadFromFile(L"Shaders/sprite.ps.hlsl","PSMain", "ps_5_0"))
    {
        return false;
    }
    constant_buffer_ = std::make_unique<ConstantBuffer>();
    if (!constant_buffer_->Initialize(device, sizeof(SpriteCBData)))
    {
        return false;
    }
    root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder rs_builder;
    rs_builder.AddCbv(0, D3D12_SHADER_VISIBILITY_ALL);
    if (!rs_builder.Build(device,root_signature_.get()))
    {
        return false;
    }

    // 4. PipelineState
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

    if (!ps_builder.Build(device,pipeline_state_.get()))
    {
        return false;
    }

    return true;
}

void SpriteRenderer::Shutdown()
{
}


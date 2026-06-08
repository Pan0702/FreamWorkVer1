#include "sky_renderer.h"

#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Resource/vertex_types.h"

namespace 
{
    
}

bool sky_renderer::Initialize(ID3D12Device* device)
{
    root_signature_ = std::make_unique<RootSignature>();
    pipeline_state_ = std::make_unique<PipelineState>();
    RootSignatureBuilder builder;
    builder
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
    if (!builder.Build(device, root_signature_.get()))
    {
        return false;
    }
    auto vs = std::make_unique<Shader>();
    if (!vs->LoadFromFile(L"Shaders/sky.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }
    auto ps = std::make_unique<Shader>();
    if (!ps->LoadFromFile(L"Shaders/sky.ps.hlsl", "PSMain", "ps_5_0"))
    {
        return false;
    }

    pipeline_state_ = std::make_unique<PipelineState>();
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = kSkyLayout;;
    layout_desc.NumElements = _countof(kSkyLayout);
    PipelineStateBuilder ps_builder;
    ps_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vs->GetBytecode())
        .SetPixelShader(ps->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetDepthEnabled(false)
        .SetAlphaBlendEnabled(false);
    if (!ps_builder.Build(device, pipeline_state_.get()))
    {
        return false;
    }

}

void sky_renderer::SetTexture()
{
}

void sky_renderer::Render()
{
}

#include "shadow_renderer.h"

#include "mesh_renderer.h"
#include "skinned_mesh_renderer.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Resource/vertex_types.h"

bool ShadowRenderer::Initialize(ID3D12Device* device, DescriptorHeap* srv_heap)
{
    shadow_root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder builder;
    builder
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX);
    if (!builder.Build(device, shadow_root_signature_.get()))
    {
        return false;
    }
    shadow_vs_ = std::make_unique<Shader>();
    if (!shadow_vs_->LoadFromFile(L"Shaders/shadow.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }

    shadow_pso_ = std::make_unique<PipelineState>();
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = kStaticVertexLayout;
    layout_desc.NumElements = _countof(kStaticVertexLayout);;
    PipelineStateBuilder ps_builder;
    ps_builder
        .SetRootSignature(shadow_root_signature_->GetRootSignature())
        .SetVertexShader(shadow_vs_->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetNumRederTarget(0)
        .SetDsvFormat(DXGI_FORMAT_D32_FLOAT)
        .SetDepthBias(10000, 1.5f);
    if (!ps_builder.Build(device, shadow_pso_.get()))
    {
        return false;
    }
    
    shadow_sk_root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder builder_sk;
    builder_sk
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddCbv(1, D3D12_SHADER_VISIBILITY_VERTEX);
    if (!builder_sk.Build(device, shadow_sk_root_signature_.get()))
    {
        return false;
    }
    shadow_sk_vs_ = std::make_unique<Shader>();
    if (!shadow_sk_vs_->LoadFromFile(L"Shaders/shadow_sk.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }
    
    shadow_sk_pso_ = std::make_unique<PipelineState>();
    D3D12_INPUT_LAYOUT_DESC layout_desc_sk = {};
    layout_desc_sk.pInputElementDescs = kSkinnedVertexLayout;
    layout_desc_sk.NumElements = _countof(kSkinnedVertexLayout);;
    PipelineStateBuilder ps_builder_sk;
    ps_builder_sk
        .SetRootSignature(shadow_sk_root_signature_->GetRootSignature())
        .SetVertexShader(shadow_sk_vs_->GetBytecode())
        .SetInputLayout(layout_desc_sk)
        .SetNumRederTarget(0)
        .SetDsvFormat(DXGI_FORMAT_D32_FLOAT)
        .SetDepthBias(10000, 1.5f);
    if (!ps_builder_sk.Build(device, shadow_sk_pso_.get()))
    {
        return false;
    }
    if (!shadow_map_.Initialize(device, 2048, srv_heap))
    {
        return false;
    }
    return true;
}

void ShadowRenderer::RenderShadowPass(RenderContext& context, const MeshRenderer* mesh,
                                      const SkinnedMeshRenderer* skinned) const
{
    auto* cmd = context.command_list;
    D3D12_RESOURCE_BARRIER to_write = {};
    to_write.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    to_write.Transition.pResource = shadow_map_.GetResource();
    to_write.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    to_write.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    to_write.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    cmd->ResourceBarrier(1, &to_write);

    //DVSをバインドしてクリア
    D3D12_CPU_DESCRIPTOR_HANDLE dsv = shadow_map_.GetDRV();
    cmd->OMSetRenderTargets(0, nullptr,FALSE, &dsv);
    cmd->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //Shadowマップサイズ
    const float s = static_cast<float>(shadow_map_.GetSize());
    D3D12_VIEWPORT vp = {0.0f, 0.0f, s, s, 0.0f, 1.0f,};
    D3D12_RECT rect = {0, 0, static_cast<LONG>(shadow_map_.GetSize()), static_cast<LONG>(shadow_map_.GetSize())};
    cmd->RSSetViewports(1, &vp);
    cmd->RSSetScissorRects(1, &rect);

    //描画
    cmd->SetGraphicsRootSignature(shadow_root_signature_->GetRootSignature());
    cmd->SetPipelineState(shadow_pso_->GetPipelineState());
    if (mesh)
    {
        mesh->SubmitDepth(context);
    }
    
    cmd->SetGraphicsRootSignature(shadow_sk_root_signature_->GetRootSignature());
    cmd->SetPipelineState(shadow_sk_pso_->GetPipelineState());
    if (skinned)
    {
        skinned->SubmitDepth(context);
    }

    //終了バリア
    D3D12_RESOURCE_BARRIER to_read = {};
    to_read.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    to_read.Transition.pResource = shadow_map_.GetResource();
    to_read.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    to_read.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    to_read.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    cmd->ResourceBarrier(1, &to_read);
}

uint32 ShadowRenderer::GetShadowMapIndex() const
{
    return shadow_map_.GetSrvIndex();
}

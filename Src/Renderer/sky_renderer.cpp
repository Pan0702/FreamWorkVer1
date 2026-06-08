#include "sky_renderer.h"

#include "render_context.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Resource/vertex_types.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/descriptor_heap.h"
#include "../Resource/mesh.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Resource/texture2D.h"
namespace 
{
    
}

bool SkyRenderer::Initialize(ID3D12Device* device)
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

    return true;
}

void SkyRenderer::SetTexture()
{
}

void SkyRenderer::Render(RenderContext& context)
{
    if (!sky_mesh_ || !sky_texture_)
    {
        return;
    }
    
    constexpr float kRadius = 500.0f;
    Mat world = Scale(Vec3(kRadius,kRadius,kRadius));
    Mat wvp = Transpose(world * context.view * context.projection);
    auto command_list = context.command_list;
    command_list->SetGraphicsRootSignature( root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
    command_list->SetDescriptorHeaps(1, heaps);
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //b0
    ConstantBufferAllocation cb = {};
    if (context.cb_allocator->Allocate(sizeof(wvp),&cb))
    {
        memcpy(cb.cpu, &wvp, sizeof(wvp));
        command_list->SetGraphicsRootConstantBufferView(0, cb.gpu);
    }
    command_list->SetGraphicsRootDescriptorTable(1,context.srv_heap->GetGpuHandle(sky_texture_->GetSrvIndex()));
    
    //球を描画
    auto vbv = sky_mesh_->GetVertexBufferView();
    command_list->IASetVertexBuffers(0, 1, &vbv);
    auto ibv = sky_mesh_->GetIndexBufferView();
    command_list->IASetIndexBuffer(&ibv);
    command_list->DrawIndexedInstanced(sky_mesh_->GetIndexCount(), 1, 0, 0, 0);
}

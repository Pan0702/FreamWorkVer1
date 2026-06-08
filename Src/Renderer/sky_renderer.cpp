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
    void BuildSphere(int stacs, int slices, std::vector<SkyVertex>& verts, std::vector<uint32>& indices)
    {
        for (int x = 0; x <= stacs; ++x)
        {
            float phi = static_cast<float>(x) / static_cast<float>(stacs) * k2PI;
            for (int y = 0; y <= slices; ++y)
            {
                float theta = static_cast<float>(y) / static_cast<float>(slices) * kPI;
                float xn = std::cos(phi) * std::sin(theta);
                float yn = std::sin(phi) * std::sin(theta);
                float zn = std::cos(theta);
                verts.push_back({ xn, yn, zn });
            }
        }
        for (int y = 0; y <= slices; ++y)
        {
            for (int x = 0; x <= stacs; ++x)
            {
                uint32 i0 = y*(slices+1)+x;
                uint32 i1 = i0+1;
                uint32 i2 = i0+(slices+1); 
                uint32 i3 = i2+1;
                indices.insert(indices.end(), { i0,i2,i1,i1,i2,i3 });
            }
        }
    }
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

    std::vector<SkyVertex> verts;
    std::vector<uint32> indices;
    BuildSphere(32, 32, verts, indices);

    VertexData vd = {};
    vd.data = verts.data();
    vd.total_size = static_cast<uint32>(verts.size() * sizeof(SkyVertex));
    vd.stride = sizeof(SkyVertex);
    IndexData id = {};
    id.data = indices.data();
    id.total_size = static_cast<uint32>(indices.size() * sizeof(uint32));
    id.format = DXGI_FORMAT_R32_UINT;
    sky_mesh_ = std::make_unique<Mesh>();
    if (!sky_mesh_->Create(device, vd, id, kSkyLayout))
    {
        return false;
    }

    return true;
}

void SkyRenderer::SetTexture(Texture2D* texture)
{
    sky_texture_ = texture;
}

void SkyRenderer::Render(const RenderContext& context) const
{
    if (!sky_mesh_ || !sky_texture_)
    {
        return;
    }

    constexpr float kRadius = 500.0f;
    Mat world = Scale(Vec3(kRadius, kRadius, kRadius));
    Mat wvp = Transpose(world * context.view * context.projection);
    auto command_list = context.command_list;
    command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    ID3D12DescriptorHeap* heaps[] = {context.srv_heap->GetHeap()};
    command_list->SetDescriptorHeaps(1, heaps);
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //b0
    ConstantBufferAllocation cb = {};
    if (context.cb_allocator->Allocate(sizeof(wvp), &cb))
    {
        memcpy(cb.cpu, &wvp, sizeof(wvp));
        command_list->SetGraphicsRootConstantBufferView(0, cb.gpu);
    }
    command_list->SetGraphicsRootDescriptorTable(1, context.srv_heap->GetGpuHandle(sky_texture_->GetSrvIndex()));

    //球を描画
    auto vbv = sky_mesh_->GetVertexBufferView();
    command_list->IASetVertexBuffers(0, 1, &vbv);
    auto ibv = sky_mesh_->GetIndexBufferView();
    command_list->IASetIndexBuffer(&ibv);
    command_list->DrawIndexedInstanced(sky_mesh_->GetIndexCount(), 1, 0, 0, 0);
}

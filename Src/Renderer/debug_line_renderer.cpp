#include "debug_line_renderer.h"
#include "../Resource/vertex_types.h"
#include <cstring>
#include "../Core/common.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "render_context.h"

bool DebugLineRenderer::CreateDynamicVertexBuffer(ID3D12Device* device, uint32_t buffer_size,
                                                  ComPtr<ID3D12Resource>& out_buffer, void*& out_mapped,
                                                  D3D12_GPU_VIRTUAL_ADDRESS& out_gpu_address)
{
    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = buffer_size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = device->CreateCommittedResource(
        &heap_props, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&out_buffer));
    if (FAILED(hr))
    {
        return false;
    }

    // 書き込み専用なので read range は空にして Map しっぱなしにする
    D3D12_RANGE read_range = {0, 0};
    hr = out_buffer->Map(0, &read_range, &out_mapped);
    if (FAILED(hr))
    {
        return false;
    }
    out_gpu_address = out_buffer->GetGPUVirtualAddress();
    return true;
}

bool DebugLineRenderer::Initialize(ID3D12Device* device)
{
    // --- シェーダ（線・三角形で共通：位置を view_proj 変換して色を渡すだけ） ---//
    vs_ = std::make_unique<Shader>();
    if (!vs_->LoadFromFile(L"Shaders/debug_line.vs.hlsl", "VSMain", "vs_5_0"))
    {
        return false;
    }

    ps_ = std::make_unique<Shader>();
    if (!ps_->LoadFromFile(L"Shaders/debug_line.ps.hlsl", "PSMain", "ps_5_0"))
    {
        return false;
    }

    // --- ルートシグネチャ：view_proj 用の CBV 1個だけ ---//
    root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder rs_builder;
    rs_builder.AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX);
    if (!rs_builder.Build(device, root_signature_.get()))
    {
        return false;
    }

    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = kDebugLineLayout;
    layout_desc.NumElements = _countof(kDebugLineLayout);

    // --- 線用 PSO：LINE トポロジ・深度書き込みON ---//
    pipeline_state_ = std::make_unique<PipelineState>();
    PipelineStateBuilder line_builder;
    line_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vs_->GetBytecode())
        .SetPixelShader(ps_->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .SetDepthEnabled(true); // ワールド空間。メッシュに隠れてほしいので true//
    if (!line_builder.Build(device, pipeline_state_.get()))
    {
        return false;
    }

    // --- 塗り用 PSO：TRIANGLE トポロジ・半透明（深度テストON / 書き込みOFF・アルファブレンド） ---//
    tri_pipeline_state_ = std::make_unique<PipelineState>();
    PipelineStateBuilder tri_builder;
    tri_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vs_->GetBytecode())
        .SetPixelShader(ps_->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .SetDepthEnabled(true)
        .SetDepthWriteEnabled(false)
        .SetAlphaBlendEnabled(true);
    if (!tri_builder.Build(device, tri_pipeline_state_.get()))
    {
        return false;
    }

    // --- 動的頂点バッファ（線・三角形でそれぞれ確保） ---//
    capacity_ = 4048;
    const uint32_t buffer_size = capacity_ * sizeof(DebugLineVertex);
    if (!CreateDynamicVertexBuffer(device, buffer_size, vertex_buffer_, mapped_, gpu_address_))
    {
        return false;
    }
    if (!CreateDynamicVertexBuffer(device, buffer_size, tri_vertex_buffer_, tri_mapped_, tri_gpu_address_))
    {
        return false;
    }

    return true;
}

void DebugLineRenderer::Shutdown()
{
    if (vertex_buffer_ && mapped_)
    {
        vertex_buffer_->Unmap(0, nullptr);
        mapped_ = nullptr;
    }
    if (tri_vertex_buffer_ && tri_mapped_)
    {
        tri_vertex_buffer_->Unmap(0, nullptr);
        tri_mapped_ = nullptr;
    }
    vertices_.clear();
    tri_vertices_.clear();
}

void DebugLineRenderer::AddLine(const Vec3& start, const Vec3& end, const Vec4& color)
{
    DebugLineVertex v0 = {{start.x, start.y, start.z}, {color.x, color.y, color.z, color.w}};
    DebugLineVertex v1 = {{end.x, end.y, end.z}, {color.x, color.y, color.z, color.w}};
    vertices_.push_back(v0);
    vertices_.push_back(v1);
}

void DebugLineRenderer::AddTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec4& color)
{
    DebugLineVertex v0 = {{a.x, a.y, a.z}, {color.x, color.y, color.z, color.w}};
    DebugLineVertex v1 = {{b.x, b.y, b.z}, {color.x, color.y, color.z, color.w}};
    DebugLineVertex v2 = {{c.x, c.y, c.z}, {color.x, color.y, color.z, color.w}};
    tri_vertices_.push_back(v0);
    tri_vertices_.push_back(v1);
    tri_vertices_.push_back(v2);
}

void DebugLineRenderer::Submit(RenderContext& context)
{
    const bool has_lines = !vertices_.empty();
    const bool has_tris = !tri_vertices_.empty();
    if (!has_lines && !has_tris)
    {
        return;
    }

    // CB に view * projection を積む（HLSL の mul(pos, M) 規約に合わせて Transpose）。線・三角形で共有//
    Mat view_proj = Transpose(context.view * context.projection);
    ConstantBufferAllocation allocation = {};
    if (!context.cb_allocator->Allocate(sizeof(view_proj), &allocation))
    {
        Clear();
        return;
    }
    memcpy(allocation.cpu, &view_proj, sizeof(view_proj));

    context.command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    context.command_list->SetGraphicsRootConstantBufferView(0, allocation.gpu);

    // --- 線（先に不透明で描いて深度を書く） ---//
    if (has_lines)
    {
        uint32_t count = static_cast<uint32_t>(vertices_.size());
        if (count > capacity_)
        {
            count = capacity_;
        }
        memcpy(mapped_, vertices_.data(), count * sizeof(DebugLineVertex));

        D3D12_VERTEX_BUFFER_VIEW vbv = {};
        vbv.BufferLocation = gpu_address_;
        vbv.SizeInBytes = count * sizeof(DebugLineVertex);
        vbv.StrideInBytes = sizeof(DebugLineVertex);

        context.command_list->SetPipelineState(pipeline_state_->GetPipelineState());
        context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        context.command_list->IASetVertexBuffers(0, 1, &vbv);
        context.command_list->DrawInstanced(count, 1, 0, 0);
    }

    // --- 塗りつぶし三角形（後から半透明でブレンド） ---//
    if (has_tris)
    {
        uint32_t count = static_cast<uint32_t>(tri_vertices_.size());
        if (count > capacity_)
        {
            count = capacity_;
        }
        memcpy(tri_mapped_, tri_vertices_.data(), count * sizeof(DebugLineVertex));

        D3D12_VERTEX_BUFFER_VIEW vbv = {};
        vbv.BufferLocation = tri_gpu_address_;
        vbv.SizeInBytes = count * sizeof(DebugLineVertex);
        vbv.StrideInBytes = sizeof(DebugLineVertex);

        context.command_list->SetPipelineState(tri_pipeline_state_->GetPipelineState());
        context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context.command_list->IASetVertexBuffers(0, 1, &vbv);
        context.command_list->DrawInstanced(count, 1, 0, 0);
    }

    Clear();
}

void DebugLineRenderer::Clear()
{
    vertices_.clear();
    tri_vertices_.clear();
}
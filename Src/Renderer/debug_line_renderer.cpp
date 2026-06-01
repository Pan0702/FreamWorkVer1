#include "debug_line_renderer.h"
#include "../Resource/vertex_types.h"
#include <cstring>
#include "../Core/common.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "render_context.h"

bool DebugLineRenderer::Initialize(ID3D12Device* device)
{
    // --- シェーダ ---//
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

    // --- PSO：LINE トポロジ指定が肝（IASetPrimitiveTopology だけでは線にならない） ---//
    pipeline_state_ = std::make_unique<PipelineState>();
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = kDebugLineLayout;
    layout_desc.NumElements = _countof(kDebugLineLayout);

    PipelineStateBuilder ps_builder;
    ps_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vs_->GetBytecode())
        .SetPixelShader(ps_->GetBytecode())
        .SetInputLayout(layout_desc)
        .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .SetDepthEnabled(true); // ワールド空間。メッシュに隠れてほしいので true//
    if (!ps_builder.Build(device, pipeline_state_.get()))
    {
        return false;
    }

    // --- 動的頂点バッファ（UPLOAD heap, Map しっぱなし） ---//
    capacity_ = 4048;
    const uint32_t buffer_size = capacity_ * sizeof(DebugLineVertex);

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
        IID_PPV_ARGS(&vertex_buffer_));
    if (FAILED(hr))
    {
        return false;
    }

    // 書き込み専用なので read range は空にして Map しっぱなしにする//
    D3D12_RANGE read_range = {0, 0};
    hr = vertex_buffer_->Map(0, &read_range, &mapped_);
    if (FAILED(hr))
    {
        return false;
    }
    gpu_address_ = vertex_buffer_->GetGPUVirtualAddress();

    return true;
}

void DebugLineRenderer::Shutdown()
{
    if (vertex_buffer_ && mapped_)
    {
        vertex_buffer_->Unmap(0, nullptr);
        mapped_ = nullptr;
    }
    vertices_.clear();
}

void DebugLineRenderer::AddLine(const Vec3& start, const Vec3& end, const Vec4& color)
{
    DebugLineVertex v0 = {{start.x, start.y, start.z}, {color.x, color.y, color.z, color.w}};
    DebugLineVertex v1 = {{end.x, end.y, end.z}, {color.x, color.y, color.z, color.w}};
    vertices_.push_back(v0);
    vertices_.push_back(v1);
}

void DebugLineRenderer::Submit(RenderContext& context)
{
    if (vertices_.empty())
    {
        return;
    }

    // capacity 超過は clamp（あふれたぶんはこのフレームでは描かない）//
    uint32_t vertex_count = static_cast<uint32_t>(vertices_.size());
    if (vertex_count > capacity_)
    {
        vertex_count = capacity_;
    }

    // CB に view * projection を積む（HLSL の mul(pos, M) 規約に合わせて Transpose）//
    Mat view_proj = Transpose(context.view * context.projection);
    ConstantBufferAllocation allocation = {};
    if (!context.cb_allocator->Allocate(sizeof(view_proj), &allocation))
    {
        Clear();
        return;
    }
    memcpy(allocation.cpu, &view_proj, sizeof(view_proj));

    // 今フレームの線分を動的 VB へ書き込む//
    memcpy(mapped_, vertices_.data(), vertex_count * sizeof(DebugLineVertex));

    D3D12_VERTEX_BUFFER_VIEW vbv = {};
    vbv.BufferLocation = gpu_address_;
    vbv.SizeInBytes = vertex_count * sizeof(DebugLineVertex);
    vbv.StrideInBytes = sizeof(DebugLineVertex);

    context.command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    context.command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    context.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    context.command_list->IASetVertexBuffers(0, 1, &vbv);
    context.command_list->SetGraphicsRootConstantBufferView(0, allocation.gpu);
    context.command_list->DrawInstanced(vertex_count, 1, 0, 0);

    Clear();
}

void DebugLineRenderer::Clear()
{
    vertices_.clear();
}
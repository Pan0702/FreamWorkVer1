#pragma once


#include "../Core/common.h"

class PipelineState
{
public:
    ID3D12PipelineState* GetPipelineState() const;

private:
    ComPtr<ID3D12PipelineState> pipeline_state_;
    friend class PipelineStateBuilder;
};

class PipelineStateBuilder
{
public:
    PipelineStateBuilder& SetRootSignature(ID3D12RootSignature* root_signature);
    PipelineStateBuilder& SetVertexShader(const D3D12_SHADER_BYTECODE& bytecode);
    PipelineStateBuilder& SetPixelShader(const D3D12_SHADER_BYTECODE& bytecode);
    PipelineStateBuilder& SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& input_layout);
    PipelineStateBuilder& SetRtvFormat(DXGI_FORMAT format);
    PipelineStateBuilder& SetDsvFormat(DXGI_FORMAT format);
    PipelineStateBuilder& SetDepthEnabled(bool enabled);
    PipelineStateBuilder& SetAlphaBlendEnabled(bool enabled);

    bool Build(ID3D12Device* device, PipelineState*out_pipeline_state) const;

private:
    ID3D12RootSignature* root_signature_ = nullptr;
    D3D12_SHADER_BYTECODE vertex_shader_ = {};
    D3D12_SHADER_BYTECODE pixel_shader_ = {};
    D3D12_INPUT_LAYOUT_DESC input_layout_ = {};

    bool depth_enable_ = true;
    bool blend_alpha_enable_ = false;

    DXGI_FORMAT rtv_format_ =
DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT dsv_format_ = DXGI_FORMAT_D32_FLOAT;
};
#pragma once

#include "../Core/common.h"

/**
 * @brief PipelineStateのデータと処理をまとめる型。
 */
class PipelineState
{
public:
    /**
     * @brief パイプラインステート を取得する。
     * @return 保持しているパイプラインステート。未作成なら nullptr。
     */
    ID3D12PipelineState* GetPipelineState() const;

private:
    ComPtr<ID3D12PipelineState> pipeline_state_;
    friend class PipelineStateBuilder;
};

/**
 * @brief PipelineStateBuilderのデータと処理をまとめる型。
 */
class PipelineStateBuilder
{
public:
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param root_signature root_signature に設定する値。
     * @return 保持している Root Signature への参照。
     */
    PipelineStateBuilder& SetRootSignature(ID3D12RootSignature* root_signature);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param bytecode bytecode に設定する値。
     * @return 保持している Vertex Shader への参照。
     */
    PipelineStateBuilder& SetVertexShader(const D3D12_SHADER_BYTECODE& bytecode);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param bytecode bytecode に設定する値。
     * @return 保持している Pixel Shader への参照。
     */
    PipelineStateBuilder& SetPixelShader(const D3D12_SHADER_BYTECODE& bytecode);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param input_layout input_layout に設定する値。
     * @return 保持している Input Layout への参照。
     */
    PipelineStateBuilder& SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& input_layout);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param format format に設定する値。
     * @return 保持している Rtv Format への参照。
     */
    PipelineStateBuilder& SetRtvFormat(DXGI_FORMAT format);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param format format に設定する値。
     * @return 保持している Dsv Format への参照。
     */
    PipelineStateBuilder& SetDsvFormat(DXGI_FORMAT format);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param enabled enabled に設定する値。
     * @return 保持している Depth Enabled への参照。
     */
    PipelineStateBuilder& SetDepthEnabled(bool enabled);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param enabled enabled に設定する値。
     * @return 保持している Alpha Blend Enabled への参照。
     */
    PipelineStateBuilder& SetAlphaBlendEnabled(bool enabled);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param type type に設定する値。
     * @return 保持している Primitive Topology Type への参照。
     */
    PipelineStateBuilder& SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param enabled enabled に設定する値。
     * @return 保持している Depth Write Enabled への参照。
     */
    PipelineStateBuilder& SetDepthWriteEnabled(bool enabled);
    /**
     * @brief 後続処理で使いやすい形にデータを組み立てる。
     * @param device 使用する D3D12 デバイス。
     * @param out_pipeline_state out_pipeline_state に設定する値。
     * @return 後続処理で使いやすい形にデータを組み立てる 場合は true。
     */
    bool Build(ID3D12Device* device, PipelineState* out_pipeline_state) const;

private:
    ID3D12RootSignature* root_signature_ = nullptr;
    D3D12_SHADER_BYTECODE vertex_shader_ = {};
    D3D12_SHADER_BYTECODE pixel_shader_ = {};
    D3D12_INPUT_LAYOUT_DESC input_layout_ = {};

    bool depth_enable_ = true;
    bool blend_alpha_enable_ = false;
    bool depth_write_ = true;

    DXGI_FORMAT rtv_format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT dsv_format_ = DXGI_FORMAT_D32_FLOAT;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

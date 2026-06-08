#pragma once

#include "../Core/common.h"

/**
 * @brief PipelineStateのデータと処理をまとめる型。
 */
class PipelineState
{
public:
    /**
     * @brief PipelineStateを取得する関数。
     * @return 戻り値。
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
     * @brief RootSignatureを設定する関数。
     * @param root_signature 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetRootSignature(ID3D12RootSignature* root_signature);
    /**
     * @brief VertexShaderを設定する関数。
     * @param bytecode 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetVertexShader(const D3D12_SHADER_BYTECODE& bytecode);
    /**
     * @brief PixelShaderを設定する関数。
     * @param bytecode 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetPixelShader(const D3D12_SHADER_BYTECODE& bytecode);
    /**
     * @brief InputLayoutを設定する関数。
     * @param input_layout 頂点入力レイアウト。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& input_layout);
    /**
     * @brief レンダーターゲットビューのフォーマットを設定する関数。
     * @param format 出力する書式文字列。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetRtvFormat(DXGI_FORMAT format);
    /**
     * @brief 深度ステンシルビューのフォーマットを設定する関数。
     * @param format 出力する書式文字列。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetDsvFormat(DXGI_FORMAT format);
    /**
     * @brief DepthEnabledを設定する関数。
     * @param enabled 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetDepthEnabled(bool enabled);
    /**
     * @brief AlphaBlendEnabledを設定する関数。
     * @param enabled 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetAlphaBlendEnabled(bool enabled);
    /**
     * @brief PrimitiveTopologyTypeを設定する関数。
     * @param type 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
    /**
     * @brief DepthWriteEnabledを設定する関数。
     * @param enabled 引数。
     * @return 戻り値。
     */
    PipelineStateBuilder& SetDepthWriteEnabled(bool enabled);
    /**
     * @brief Buildを行う関数。
     * @param device DirectX 12 デバイス。
     * @param out_pipeline_state 引数。
     * @return 条件を満たす場合は true。
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

#include "pipeline_state.h"
#include "shader.h"

ID3D12PipelineState* PipelineState::GetPipelineState() const
{
    return pipeline_state_.Get();
}

PipelineStateBuilder& PipelineStateBuilder::SetRootSignature(ID3D12RootSignature* root_signature)
{
    root_signature_ = root_signature;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetVertexShader(const D3D12_SHADER_BYTECODE& bytecode)
{
    vertex_shader_ = bytecode;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetPixelShader(const D3D12_SHADER_BYTECODE& bytecode)
{
    pixel_shader_ = bytecode;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& input_layout)
{
    input_layout_ = input_layout;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetRtvFormat(DXGI_FORMAT format)
{
    rtv_format_ = format;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetDsvFormat(DXGI_FORMAT format)
{
    dsv_format_ = format;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetDepthEnabled(bool enabled)
{
    depth_enable_ = enabled;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetAlphaBlendEnabled(bool enabled)
{
    blend_alpha_enable_ = enabled;
    return *this;
}

PipelineStateBuilder& PipelineStateBuilder::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
    primitive_topology_type_ = type;
    return *this;
}

bool PipelineStateBuilder::Build(ID3D12Device* device, PipelineState* out_pipeline_state) const
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.InputLayout.pInputElementDescs = input_layout_.pInputElementDescs;
    desc.InputLayout.NumElements = input_layout_.NumElements;
    desc.pRootSignature = root_signature_;
    desc.VS = vertex_shader_;
    desc.PS = pixel_shader_;
    //全部のサンプルを使う
    desc.SampleMask = UINT_MAX;
    //三角形の描画をしろっていう宣言
    desc.PrimitiveTopologyType = primitive_topology_type_;
    //書き先（RTV）の枚数
    desc.NumRenderTargets = 1;
    //↑の色のフォーマットSwapChainと違う色だと失敗する
    desc.RTVFormats[0] = rtv_format_;
    //MSAA(アンチエイリアス)なし
    //0は失敗、2以上で使用、４がよくつかわれる値
    desc.SampleDesc.Count = 1;

    //深度
    {
        //深度テストする
        desc.DepthStencilState.DepthEnable = depth_enable_ ? TRUE : FALSE;

        desc.DepthStencilState.DepthWriteMask =
            depth_enable_ ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

        desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        //ステンシルテストしない
        desc.DepthStencilState.StencilEnable = FALSE;
        //深度バッファは保持してない
        desc.DSVFormat = dsv_format_;
    }

    //色の混ぜ方
    {
        //MSAA用のα処理
        desc.BlendState.AlphaToCoverageEnable = FALSE;
        //RT(RenderTargetGPUが描き込む先のテクスチャ)
        //が複数枚あるとき1枚ごとに設定を分けるか
        desc.BlendState.IndependentBlendEnable = FALSE;

        auto& rt0 = desc.BlendState.RenderTarget[0];
        //αブレンドするか
        if (blend_alpha_enable_)
        {
            rt0.BlendEnable = TRUE;
            rt0.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt0.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            rt0.BlendOp = D3D12_BLEND_OP_ADD;
            rt0.SrcBlendAlpha = D3D12_BLEND_ONE;
            rt0.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            rt0.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        }else
        {
            rt0.BlendEnable = FALSE;
        }
        //色を合成するか
        rt0.LogicOpEnable = FALSE;
        //RGB全体に書き込む
        rt0.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }

    //ラスタライザ(3D三角形から2Dpixel変換ツール)
    {
        //三角形の塗り方
        desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        //裏向きの面を捨てるか
        desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        //どっちの向きがおもてか
        desc.RasterizerState.FrontCounterClockwise = FALSE;
        //深度値を整数でずらす
        desc.RasterizerState.DepthBias = 0;
        //DepthBiasの上限
        desc.RasterizerState.DepthBiasClamp = 0.0f;
        //傾きに応じだdepthBias
        desc.RasterizerState.SlopeScaledDepthBias = 0.0f;
        //視野買いをカットするか
        desc.RasterizerState.DepthClipEnable = TRUE;
        //MASSを有効化(2以上の時だけtrueにする) 
        desc.RasterizerState.MultisampleEnable = FALSE;
        //線描画時のアンチエイリアス
        desc.RasterizerState.AntialiasedLineEnable = FALSE;
        //UAVとMSAAの組み合わせ用
        desc.RasterizerState.ForcedSampleCount = 0;
        //保守的すたライズ
        desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }
    HRESULT hr = device->CreateGraphicsPipelineState(&desc,IID_PPV_ARGS(&out_pipeline_state->pipeline_state_));
    return SUCCEEDED(hr);
}

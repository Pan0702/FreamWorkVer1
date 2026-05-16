#include "pipeline_state.h"
#include "shader.h"
bool PipelineState::Initialize(ID3D12Device* device, ID3D12RootSignature* root_signature, const Shader& vertex_shader,
                               const Shader& pixel_shader)
{
    D3D12_INPUT_ELEMENT_DESC input_elements[] = {
        {
            .SemanticName = "POSITION",.SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        },
        {
            .SemanticName = "COLOR",.SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        },
    };
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.InputLayout.pInputElementDescs = input_elements;
    desc.InputLayout.NumElements = _countof(input_elements);
    desc.pRootSignature = root_signature;
    desc.VS = vertex_shader.GetBytecode();
    desc.PS = pixel_shader.GetBytecode();
    //全部のサンプルを使う
    desc.SampleMask = UINT_MAX;
    //三角形の描画をしろっていう宣言
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //書き先（RTV）の枚数
    desc.NumRenderTargets = 1;
    //↑の色のフォーマットSwapChainと違う色だと失敗する
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    //MSAA(アンチエイリアス)なし
    //0は失敗、2以上で使用、４がよくつかわれる値
    desc.SampleDesc.Count = 1;
    
    //深度
    {
        //深度テストしない
        desc.DepthStencilState.DepthEnable = FALSE;
        //ステンシルテストしない
        desc.DepthStencilState.StencilEnable = FALSE;
        //深度バッファは保持してない
        desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
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
        rt0.BlendEnable = FALSE;
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
        desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
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
    HRESULT hr = device->CreateGraphicsPipelineState(&desc,IID_PPV_ARGS(&pipeline_state_));
    return SUCCEEDED(hr);
}

ID3D12PipelineState* PipelineState::GetPipelineState() const
{
    return pipeline_state_.Get();
}

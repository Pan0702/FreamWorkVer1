#pragma once
#include <memory>
#include "../Core/common.h"

struct RenderContext;
class Texture2D;
class PipelineState;
class RootSignature;
class Mesh;
/**
 * @brief SkyRendererのデータと処理をまとめる型。
 */
class SkyRenderer
{
public:
    /**
     * @brief 値を初期化する。
     */
    SkyRenderer();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~SkyRenderer();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param texture 使用するテクスチャ。
     */
    void SetTexture(Texture2D* texture);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void Render(const RenderContext& context) const;
private:
    std::unique_ptr<Mesh> sky_mesh_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;   
    Texture2D* sky_texture_ = nullptr;  
};

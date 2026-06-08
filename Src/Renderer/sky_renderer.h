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
     * @brief SkyRendererを初期化するコンストラクタ。
     */
    SkyRenderer();
    /**
     * @brief SkyRendererの終了処理を行うデストラクタ。
     */
    ~SkyRenderer();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief Textureを設定する関数。
     * @param texture 描画に使うテクスチャ。
     */
    void SetTexture(Texture2D* texture);
    /**
     * @brief 登録済みの描画対象を描画する関数。
     * @param context 共有コンテキスト。
     */
    void Render(const RenderContext& context) const;
private:
    std::unique_ptr<Mesh> sky_mesh_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;   
    Texture2D* sky_texture_ = nullptr;  
};

#pragma once
#include "frame_snap.h"
#include "render_context.h"
#include "../Graphics/command_list.h"
#include "../Graphics/shadow_map.h"

class SkinnedMeshRenderer;
class MeshRenderer;
class PipelineState;
class Shader;
class RootSignature;

/**
 * @brief シャドウマップへの深度描画を実行する。
 */
class ShadowRenderer
{
public:
    /**
     * @brief シャドウ描画に必要なRS、Shader、PSO、ShadowMapを初期化する
     * @param device リソースに使うデバイス。
     * @param srv_heap ShadowMapのSRVを確保する共通SRVヒープ
     * @return 
     */
    bool Initialize(ID3D12Device* device, DescriptorHeap* srv_heap);


    /**
     * @brief ライト視点から深度描画し、シャドウマップを更新する・
     * @param context 共通Context
     * @param mesh 深度描画するレンダラー
     * @param skinned 深度描画するスキンメッシュレンダラー
     * @param frame
     */
    void RenderShadowPass( RenderContext& context, MeshRenderer* mesh,
                          SkinnedMeshRenderer* skinned, FrameSnap& frame);

    /**
     * @brief 作成したシャドウマップのSRVインデックスを取得する。
     * @return 通常描画でシャドウマップを参照するためのSRVヒープインデックス
     */
    uint32 GetShadowMapIndex() const;

private:
    // 通常メッシュ用の深度描画パイプライン。
    std::unique_ptr<RootSignature> shadow_root_signature_;
    std::unique_ptr<Shader> shadow_vs_;
    std::unique_ptr<PipelineState> shadow_pso_;

    // スキンメッシュ用の深度描画パイプライン。
    std::unique_ptr<RootSignature> shadow_sk_root_signature_;
    std::unique_ptr<Shader> shadow_sk_vs_;
    std::unique_ptr<PipelineState> shadow_sk_pso_;

    // スキンメッシュ用の深度描画パイプライン。
    ShadowMap shadow_map_;
};

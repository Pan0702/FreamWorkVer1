#pragma once
#include <vector>

#include "../Core/common.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Graphics/vertex_buffer.h"
#include "sprite_draw_command.h"

class ConstantBufferAllocator;
class DescriptorHeap;
struct RenderContext;
class SpriteComponent;
/**
 * @brief SpriteWorldCBDataのデータと処理をまとめる型。
 */
struct SpriteWorldCBData
{
    Mat wvp;
    Vec4 color;
    Vec4 src_rect;
    Vec4 options;
};
/**
 * @brief SpriteRendererのデータと処理をまとめる型。
 */
class SpriteRenderer
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief 対象を管理リストへ登録する。
     * @param component 登録または解除する Component。
     */
    void Register(SpriteComponent* component);
    /**
     * @brief 対象を管理リストから外す。
     * @param component 登録または解除する Component。
     */
    void Unregister(SpriteComponent* component);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param command command に設定する値。
     */
    void DrawImmediate(const SpriteDrawCommand& command);
    /**
     * @brief 登録済みの対象から今回処理する要素を集める。
     */
    void Collect();
    /**
     * @brief 描画順が安定するようにコマンドを並べ替える。
     */
    void Sort();
    /**
     * @brief 収集済みコマンドを GPU コマンドリストへ書き込む。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void Submit(RenderContext& context);

private:
    /**
     * @brief 1 件分の描画コマンドを GPU コマンドリストへ書き込む。
     * @param context 描画や登録に使う共有コンテキスト。
     * @param command command に設定する値。
     */
    void SubmitCommand(RenderContext& context, const SpriteDrawCommand& command);

    std::unique_ptr<VertexBuffer> quad_vb_;
    std::unique_ptr<IndexBuffer> quad_ib_;
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader> pixel_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    std::vector<SpriteComponent*> registered_;
    std::vector<SpriteDrawCommand> draw_commands_;
    std::vector<SpriteDrawCommand> immediate_commands_;
};

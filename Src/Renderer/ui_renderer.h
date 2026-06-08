#pragma once
#include <memory>
#include <vector>
#include <memory>
#include <vector>
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Graphics/vertex_buffer.h"
#include "sprite_draw_command.h"
#include "../Engine/Components/sprite_component.h"

struct RenderContext;
/**
 * @brief UISpriteCBDataのデータと処理をまとめる型。
 */
struct UISpriteCBData
{
    Vec2 sprite_pos;
    Vec2 sprite_size;
    Vec4 color;
    Vec2 screen_size;   
    float rotation;
    float use_texture;
};
/**
 * @brief UIRendererのデータと処理をまとめる型。
 */
class UIRenderer
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief Registerを行う関数。
     * @param component 引数。
     */
    void Register(SpriteComponent* component);
    /**
     * @brief Unregisterを行う関数。
     * @param component 引数。
     */
    void Unregister(SpriteComponent* component);
    /**
     * @brief DrawImmediateを行う関数。
     * @param command 引数。
     */
    void DrawImmediate(const SpriteDrawCommand& command);
    /**
     * @brief Collectを行う関数。
     */
    void Collect();
    /**
     * @brief Sortを行う関数。
     */
    void Sort();
    /**
     * @brief Submitを行う関数。
     * @param context 共有コンテキスト。
     */
    void Submit(RenderContext& context);

private:
    /**
     * @brief SubmitCommandを行う関数。
     * @param context 共有コンテキスト。
     * @param command 引数。
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

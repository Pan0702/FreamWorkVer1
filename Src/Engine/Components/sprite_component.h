#pragma once
#include "../../Core/Math/my_math.h"
#include "../component.h"

class SpriteRenderer;
class Texture2D;
class UIRenderer;

/**
 * @brief SpriteSpaceのデータと処理をまとめる型。
 */
enum class SpriteSpace
{
    kWorld,
    kScreen,
};

/**
 * @brief SpriteComponentのデータと処理をまとめる型。
 */
class SpriteComponent : public Component
{
public:
    /**
     * @brief 値を初期化する。
     */
    SpriteComponent() = default;
    /**
     * @brief 値を初期化する。
     * @param texture 使用するテクスチャ。
     * @param space スプライトを描画する座標空間。
     */
    SpriteComponent(Texture2D* texture, SpriteSpace space);
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~SpriteComponent() override;

    /**
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 共有コンテキストから登録を外し、システム側の参照を切る。
     */
    void OnDetach() override;

    Texture2D* texture = nullptr;
    Vec2 position = {0.0f, 0.0f};
    Vec2 size = {64.0f, 64.0f};
    Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec4 src_rect = {0.0f, 0.0f, 1.0f, 1.0f};
    SpriteSpace space = SpriteSpace::kWorld;

private:
    SpriteRenderer* sprite_renderer_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
};

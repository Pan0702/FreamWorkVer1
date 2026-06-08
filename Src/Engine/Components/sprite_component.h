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
    SpriteComponent() = default;
    /**
     * @brief SpriteComponentを初期化するコンストラクタ。
     * @param texture 描画に使うテクスチャ。
     * @param space 引数。
     */
    SpriteComponent(Texture2D* texture, SpriteSpace space);
    ~SpriteComponent() override;

    /**
     * @brief OnAttachを行う関数。
     * @param context 共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief OnDetachを行う関数。
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

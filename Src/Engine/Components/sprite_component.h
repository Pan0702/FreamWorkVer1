#pragma once
#include "../../Core/Math/my_math.h"
#include "../component.h"

class SpriteRenderer;
class Texture2D;
class UIRenderer;

/**
 * @brief SpriteSpaceのデータと処理をまとめる型。
 */
enum class SpriteSpace : uint8
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

    /**
     * @brief wとhを使ってWindowの大きさに対する相対割合を求める。
     * @param w 横幅
     * @param h 高さ
     */
    void SetSize(float w, float h);

    /**
    * @brief wとhを使ってWindowの大きさに対する相対割合を求める。
    * @param w 横幅
    * @param h 高さ
    */
    void SetPos(float w, float h);
    /**
     * @brief 相対割合を返す
     * @return size_rate_を返す。
     */
    const Vec2& GetSize() const;

    /**
     * 相対位置を使うかのフラグを返す
     * @return is_relative_pos_を返す。
     */
    bool GetRelativeSizeFlag() const;

    /**
 * @brief 相対割合を返す
 * @return pso_rate_を返す。
 */
    const Vec2& GetPos() const;

    /**
     * 相対位置を使うかのフラグを返す
     * @return is_relative_pos_を返す。
     */
    bool GetRelativePosFlag() const;

    /**
     * 現在の表示フラグを取得。
     * @return 
     */
    bool GetVisible() const;

    /**
     * 表示するかしないか
     * @param visible flag
     */
    void SetVisible(bool visible);

    Texture2D* texture = nullptr;
    Vec2 position = {0.0f, 0.0f};
    Vec2 size_ = {64.0f, 64.0f};
    Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec4 src_rect = {0.0f, 0.0f, 1.0f, 1.0f};
    SpriteSpace space = SpriteSpace::kWorld;
    int sort_key = 0;

private:
    SpriteRenderer* sprite_renderer_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
    Vec2 size_rate_;
    Vec2 pos_rate_;
    bool is_relative_size_ = false;
    bool is_relative_pos_ = false;
    bool visible_ = true;
};

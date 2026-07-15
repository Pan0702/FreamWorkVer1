#pragma once
#include "../../Engine/actor.h"

class SpriteComponent;

/**
 * @brief クリア画面で選択できる操作を表す。
 */
enum class ClearButton : uint8
{
    kRestart = 0,
    kSelect,
};
/**
 * @brief ステージクリア画面のユーザーインターフェースを表示する。
 */
class Clear : public Actor
{
public:
    /**
     * @brief クリア画面のアクターとスプライトを生成する。
     */
    Clear();
    /**
     * @brief クリア画面のユーザーインターフェースを初期化する。
     */
    void Begin() override;
    /**
     * @brief クリア画面を更新し、入力を処理する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief クリア画面を表示または非表示にする。
     * @param visible クリア画面を表示する場合は true。
     */
    void SetVisible(bool visible);
private:
    /**
     * @brief プレイヤー入力に応じて選択中のボタンを変更する。
     */
    void Input() ;
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* cur_texture_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
    int button_index_ = 0;
    bool visible_ = false;
};

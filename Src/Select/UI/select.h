#pragma once
#include "../../Engine/actor.h"
class SpriteComponent;
/**
 * @brief レベル選択画面のユーザーインターフェースを提供する。
 */
class Select : public Actor
{
public:
    /**
     * @brief 選択画面のアクターとスプライトを生成する。
     */
    Select();
    /**
     * @brief 選択画面のユーザーインターフェースを初期化する。
     */
    void Begin() override;
    /**
     * @brief 選択画面を更新し、入力を処理する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
private:
    /**
     * @brief プレイヤー入力に応じて選択中のボタンを変更する。
     */
    void Input();
    int button_index_ = 0;
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* cur_texture_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
};

#pragma once
#include "../../Engine/actor.h"

class SpriteComponent;

/**
 * @brief タイトル画面のユーザーインターフェースを表示する。
 */
class Title : public Actor
{
public:
    /**
     * @brief タイトル画面のアクターとスプライトを生成する。
     */
    Title();
    /**
     * @brief タイトル画面のアニメーションと入力処理を更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
private:
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
};

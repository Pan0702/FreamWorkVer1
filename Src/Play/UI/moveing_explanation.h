#pragma once
#include "../../Engine/actor.h"

/**
 * @brief 移動方法の説明画像を表示する。
 */
class Explanation : public Actor
{
public:
    /**
     * @brief この説明で使用する画像パスを保持する。
     * @param path 説明画像へのパス。
     */
    Explanation(std::wstring_view path);
    /**
     * @brief アクターの有効化後にスプライトを生成する。
     */
    void Begin() override;
private:
    class SpriteComponent* explanation_ = nullptr;
    std::wstring path_;
};

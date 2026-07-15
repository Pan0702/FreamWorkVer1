#pragma once
#include "transition_effect.h"
#include "../../Debug/debug.h"

/**
 * @brief 全画面フェード遷移を描画する。
 */
class Fade : public ITransitionEffect
{
public:
    /**
     * @brief 進行度に応じた不透明度の黒オーバーレイを描画する。
     * @param progress 0から1の範囲に正規化したフェードの不透明度。
     */
    void Draw(float progress) override
    {
        Debug::Get().DrawBox(Vec2(0,0),Vec2(kWindowWidth,kWindwoHeight),
                             Vec4(0,0,0,progress),0.0f);
    }
};


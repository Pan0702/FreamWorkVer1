#pragma once
#include "../Core/Math/my_math.h"
#include "../Engine/component.h"
class Texture2D;
/**
 * @brief DebugComponentのデータと処理をまとめる型。
 */
class DebugComponent : public Component
{
public:
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void Render();
    
private:
    float time_ = 0.0f;
    
};

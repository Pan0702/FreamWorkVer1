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
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    void Tick(float dt) override;
    /**
     * @brief 登録済みの描画対象を描画する関数。
     */
    void Render();
    
private:
    float time_ = 0.0f;
    
};

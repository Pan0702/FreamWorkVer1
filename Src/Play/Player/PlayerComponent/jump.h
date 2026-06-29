#pragma once
#include "state_component.h"

// PlayerJump に関係する状態と振る舞いをまとめる型。
class PlayerJump : public StateComponentBase
{
public:
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~PlayerJump();
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void OnEnter() override;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     * @param input 状態コンポーネントへ渡すプレイヤー入力。
     */
    void Tick(float dt, PlayerInput& input) override;
    /**
     * @brief 終了または遷移前に必要な後始末を行う。
     */
    void OnExit() override;

private:
    float vel_y_;
};

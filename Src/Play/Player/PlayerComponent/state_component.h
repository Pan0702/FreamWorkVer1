#pragma once
struct PlayerInput;
class Player;

// StateComponentBase に関係する状態と振る舞いをまとめる型。
class StateComponentBase
{
public:
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    virtual ~StateComponentBase() = default;
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    virtual void OnEnter() = 0;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     * @param input 状態コンポーネントへ渡すプレイヤー入力。
     */
    virtual void Tick(float dt, PlayerInput& input) = 0;
    /**
     * @brief 終了または遷移前に必要な後始末を行う。
     */
    virtual void OnExit() = 0;

protected:
};

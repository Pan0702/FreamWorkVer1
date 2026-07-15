#pragma once
#include <memory>
#include "../Core/Math/my_math.h"
#include "Transition/fade.h"
/**
 * @brief レベル遷移の現在の段階を表す。
 */
enum class TransitionPhase : uint8
{
    kNone,
    kIn,
    kOut,
};

/**
 * @brief レベル切り替え時に使用する演出を制御する。
 */
class LevelTransition
{
public:
    /**
     * @brief 遷移時に使用する視覚効果を設定する。
     * @param effect 所有して描画する効果のインスタンス。
     */
    void SetEffect(std::unique_ptr<ITransitionEffect> effect);
    /**
     * @brief 遷移を開始する。
     */
    void Start();
    /**
     * @brief 遷移の進行度を更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt);
    /**
     * @brief 現在の遷移効果を描画する。
     */
    void Draw();
    /**
     * @brief このフレームで画面が覆われたかどうかを返す。
     * @return 画面を覆う処理が完了したフレームだけ true。
     */
    bool GetJustCovered() const;
    /**
     * @brief 遷移が実行中かどうかを返す。
     * @return 遷移の実行中は true。
     */
    bool IsPlaying() const;
private:
    float time_ = 0.0f; 
    float progress_ = 0.0f;
    const float duration_ = 0.4f;
    TransitionPhase phase_ = TransitionPhase::kNone;
    std::unique_ptr<ITransitionEffect> effect_;
    bool just_covered_ = false;
};

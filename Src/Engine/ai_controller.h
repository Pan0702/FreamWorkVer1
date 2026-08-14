#pragma once
#include "actor.h"
#include "controller.h"

/**
 * @brief 憑依した Character を NavMesh 上で自動的に移動させる Controller。
 *
 * 追跡対象の位置を目的地として経路を求め、経路上のウェイポイントへ向かう
 * 移動入力を毎フレーム Character へ渡す。経路の再計算は一定間隔、または
 * 対象が前回の目的地から大きく動いたときに行う。
 */
class AiController : public Controller
{
public:
    /**
     * @brief 指定したアクターの追跡を開始する。
     *
     * 憑依済みの Character に NavigationAgentComponent を取り付ける。
     * そのため Possess の後に呼び出さなければならない。
     * @param actor 追跡するアクター。
     */
    void MoveToActor(Actor* actor);
    /**
     * @brief 追跡を終了し、保持している経路を破棄する。
     */
    void StopMovement();

private:
    /**
     * @brief 経路の再計算とウェイポイント追従を 1 フレーム分進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;

    Actor* target_ = nullptr;
    class NavigationAgentComponent* nav_agent_ = nullptr;
    Vec3 last_goal_;              // 前回 SetDestination に渡した位置
    float repath_timer_ = 0.0f;   // 前回の経路再計算からの経過秒数
    bool has_goal_ = false;       // 一度でも経路探索に成功したか
};

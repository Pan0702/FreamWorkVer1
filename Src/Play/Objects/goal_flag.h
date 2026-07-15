#pragma once
#include "../../Engine/actor.h"

/**
 * @brief プレイヤーがステージゴールへ到達したことを検出する。
 */
class GoalFlag : public Actor
{
public:
    /**
     * @brief 既定位置にゴールフラグを生成する。
     */
    GoalFlag();
    /**
     * @brief 指定位置にゴールフラグを生成する。
     * @param pos ゴールフラグのワールド座標。
     */
    GoalFlag(const Vec3& pos);
    /**
     * @brief コライダーがゴールトリガーへ入ったときの処理を行う。
     * @param c1 重なり判定ペアの1つ目のコライダー。
     * @param c2 重なり判定ペアの2つ目のコライダー。
     */
    void OnBeginOverlap(const ColliderComponent* c1,const ColliderComponent* c2);
private:
    std::unique_ptr<MaterialSlot> materials_;

};

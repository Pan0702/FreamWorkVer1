#pragma once
#include "../../Engine/actor.h"

class Cube;
class Player;

/**
 * @brief 2地点間を移動し、乗っているプレイヤーを運ぶ。
 */
class MovingCube : public Actor
{
public:
    /**
     * @brief 既定位置に移動キューブを生成する。
     */
    MovingCube();
    /**
     * @brief 2地点間を移動するキューブを生成する。
     * @param pos 初期ワールド座標。
     * @param target 移動先のワールド座標。
     */
    MovingCube(const Vec3& pos, const Vec3& target);
    /**
     * @brief 移動処理と乗っているプレイヤーの位置を更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief 移動速度を設定する。
     * @param speed 1秒あたりの移動距離。
     */
    void SetSpeed(float speed);
private:
    /**
     * @brief 移動状態と描画リソースを初期化する。
     */
    void Init();
/**
     * @brief プレイヤーがこのキューブに乗っているかどうかを返す。
     * @param player_pos 現在のプレイヤーのワールド座標。
     * @return プレイヤーがキューブに乗っている場合は true。
     */
    bool IsRiding(const Vec3& player_pos) const;

    std::unique_ptr<MaterialSlot> materials_;
    Player* player_ = nullptr;
    Vec3 base_pos_;
    Vec3 target_pos_;
    float t_ = 0.0f;
    float dir_ = 1.0f;
    float waiting_time_ = 0.0f;
    float speed_ = 2.0f;
    const float kWaitingTimeMax = 2.0f;
};

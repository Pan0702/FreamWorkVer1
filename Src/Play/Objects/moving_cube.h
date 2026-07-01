#pragma once
#include "../../Engine/actor.h"

class Cube;
class Player;

class MovingCube : public Actor
{
public:
    MovingCube();
    MovingCube(const Vec3& pos, const Vec3& target);
    void Tick(float dt) override;
    void SetSpeed(float speed);
private:
    void Init();
    // プレイヤーが床の天面に乗っているかを位置から判定する。
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

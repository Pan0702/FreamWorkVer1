#pragma once
#include "../../Engine/actor.h"

class MovingCube : public Actor
{
public:
    MovingCube();
    MovingCube(const Vec3& pos,int range = 10);
    void Tick(float dt) override;
    
private:
    int moving_range_;
    Vec3 base_pos_;
    float waiting_time_ = 0.0f;
    constexpr float kQaitingTimeMax = 2.0f;
    constexpr float kMovingSpeed = 2.0f;
    float speed_ = kMovingSpeed;
};

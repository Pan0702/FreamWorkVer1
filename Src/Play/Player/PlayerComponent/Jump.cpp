#include "Jump.h"

#include "../player.h"

namespace 
{
    constexpr float kApexHeight = 4.233f;
    constexpr float kApexTime = 0.373f;
    constexpr float kFallMul = 0.8f;
    constexpr float kJumpVel = 2.0f * kApexHeight / kApexTime;
    constexpr float kGravityUp = 2.0f * kApexHeight / (kApexTime * kApexTime);
}

PlayerJump::~PlayerJump() = default;

void PlayerJump::OnEnter()
{
    vel_y_ = kJumpVel;
}

void PlayerJump::Tick(float dt,PlayerInput& input)
{
    float g = (vel_y_ > 0.0f) ? kGravityUp : kGravityUp * kFallMul;
    vel_y_ -= g * dt;
    input.move_dir.y = vel_y_ * dt;
}

void PlayerJump::OnExit()
{
}

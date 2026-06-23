#include "jump.h"

#include "../player.h"
#include "../player_common.h"
PlayerJump::~PlayerJump() = default;

void PlayerJump::OnEnter()
{
    vel_y_ = kJumpVel;
}

void PlayerJump::Tick(float dt,PlayerInput& input)
{
    if (vel_y_ <= 0.0f)
    {
        input.jump = false;
        return;
    }
    vel_y_ -= kGravityUp * dt;
    input.move_amount.y = vel_y_ * dt;
}

void PlayerJump::OnExit()
{
}

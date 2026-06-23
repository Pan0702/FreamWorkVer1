#include "movement.h"

#include <cmath>

#include "../player.h"

PlayerMovement::~PlayerMovement() = default;

void PlayerMovement::OnEnter()
{
}

void PlayerMovement::Tick(float dt, PlayerInput& input)
{
    const float current_yaw = input.yaw;
    const float target_yaw = std::atan2f(input.move_dir.x, input.move_dir.z);
    const float delta = NormalizeAngleRadSigned(target_yaw - current_yaw);
    constexpr float rot_speed = 10.0f;
    const float step = rot_speed * dt;

    if (std::abs(delta) > step)
    {
        input.move_dir.y += delta > 0.0f ? step : -step;
        input.move_dir.y = NormalizeAngleRad(input.move_dir.y);
    }
    else
    {
        input.move_dir.y = NormalizeAngleRad(input.move_dir.y + delta);
    }
}


void PlayerMovement::OnExit()
{
}

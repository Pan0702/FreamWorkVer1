#include "movement.h"

#include <cmath>

#include "../player.h"

void PlayerMovement::OnEnter()
{
}

void PlayerMovement::Tick(float dt, const PlayerInput& input)
{
    bool is_moving = input.move_dir.LengthSquared() > 1e-6f;
    
    if (is_moving)
    {
        const float current_yaw = transform_.rotation.y;
        const float target_yaw = std::atan2f(input.move_dir.x, input.move_dir.z);
        const float delta =     NormalizeAngleRadSigned(target_yaw - current_yaw);
        constexpr float rot_speed = 10.0f;
        const float step = rot_speed * dt;

        if (std::abs(delta) > step)
        {
            transform_.rotation.y += delta > 0.0f ? step : -step;
            transform_.rotation.y = NormalizeAngleRad(transform_.rotation.y);
        }
        else
        {
            transform_.rotation.y = NormalizeAngleRad(transform_.rotation.y + delta);
        }
    }

}


void PlayerMovement::OnExit()
{
}

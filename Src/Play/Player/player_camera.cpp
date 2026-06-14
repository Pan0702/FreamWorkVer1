#include "player_camera.h"

#include "player.h"

namespace
{
    const Vec3 kCamPos = Vec3(0.0f, 5.0f, -10.0f);
}
void PlayerCamera::Begin()
{
    player_ = GetWorld()->FindActor<Player>();
    camera_ = &game_main->GetCamera();
    Actor::Begin();
}

void PlayerCamera::Tick(float dt)
{
    const Mat rot_y = RotateY(player_->GetTransform().rotation.y);
    Vec3 com_pos = player_->GetTransform().position + kCamPos;
    camera_->pos_ = com_pos;
    camera_->look_ = player_->GetTransform().position;
    Actor::Tick(dt);
}

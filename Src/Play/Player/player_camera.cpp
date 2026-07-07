#include "player_camera.h"

#include <algorithm>

#include "player.h"
#include "../../Engine/Components/skeletal_mesh.h"

namespace
{
    const Vec3 kCamOffset = Vec3(0.0f, 5.0f, -10.0f);
    constexpr float kSensitivity = 0.003f; // Š´“x                    
    constexpr float kPitchMin = -1.0f; // Œ©ã‚°ŒÀŠE(rad)         
    constexpr float kPitchMax = 0.5f; // Œ©‰º‚ë‚µŒÀŠE(rad)
}

float PlayerCamera::GetYaw() const
{
    return cam_rad_.y;
}

void PlayerCamera::Begin()
{
    player_ = GetWorld()->FindActor<Player>();
    camera_ = &game_main->GetCamera();
    Actor::Begin();
}

void PlayerCamera::Tick(float dt)
{
    Input();
    const Mat rot = RotateX(cam_rad_.x) * RotateY(cam_rad_.y);
    const Vec3 offset = TransformVector(rot, kCamOffset);
    auto* s = player_->GetComponent<SkeletalMeshComponent>()->GetSkeltalMesh();

    float scale_y = (s->GetBounds().max.y - s->GetBounds().min.y) * kHalfSize ;
    Vec3 pl_pos = Vec3(player_->GetTransform().position.x,
                       player_->GetTransform().position.y+ 1.0f * (scale_y * player_->GetTransform().scale.y),
                       player_->GetTransform().position.z);
    Vec3 com_pos = pl_pos + offset;
    if (com_pos.y < 0.0f)
    {
        com_pos.y = 0.0f;
    }
    camera_->pos_ = com_pos;
    camera_->look_ = pl_pos;
    Actor::Tick(dt);
}


void PlayerCamera::Input()
{
    auto in = game_main->GetInput();
    cam_rad_.y += in.GetMouseDeltaX() * kSensitivity;
    cam_rad_.x += in.GetMouseDeltaY() * kSensitivity;
    cam_rad_.x = std::clamp(cam_rad_.x, kPitchMin, kPitchMax);
}

#include "player.h"
#include "player_camera.h"
#include "../../Engine/Components/animation_component.h"
#include "../../Game/GameMain.h"

Player::Player() : Character("Assets/Mesh/remy.skmesh")
{
    animation_->AddAnimation(CharaAnim::kRun, AnimatorManager::Get().Load("Assets/Animation/run.anim"), true);
    animation_->AddAnimation(CharaAnim::kIdle, AnimatorManager::Get().Load("Assets/Animation/idle.anim"), true);
    animation_->AddAnimation(CharaAnim::kJump, AnimatorManager::Get().Load("Assets/Animation/jump_up.anim"), false);
    animation_->AddAnimation(CharaAnim::kFall, AnimatorManager::Get().Load("Assets/Animation/fall.anim"), true);
    animation_->AddAnimation(CharaAnim::kLanding, AnimatorManager::Get().Load("Assets/Animation/landing.anim"), false);
    
    animation_->Play(CharaAnim::kIdle);
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
}

void Player::Begin()
{
    
    transform_.position = Vec3(0, 0, 0);
    use_tick_ = true;
    camera_ = GetWorld()->FindActor<PlayerCamera>();
    Character::Begin();
}

void Player::Tick(float dt)
{
    // WASD → カメラ相対のワールド方向 (現 Input() 137-156行の計算)
    Vec3 local{};
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kDown)) local.z += 1.0f;
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kDown)) local.z -= 1.0f;
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown)) local.x += 1.0f;
    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown)) local.x -= 1.0f;

    Vec3 world_dir{};
    if (local.LengthSquared() > kEpsilon)
    {
        if (!camera_) camera_ = GetWorld()->FindActor<PlayerCamera>();
        const float cam_yaw = camera_ ? camera_->GetYaw() : 0.0f;
        world_dir = TransformVector(RotateY(cam_yaw), local).Normalized();
    }
    if (world_dir.LengthSquared() > kEpsilon)
    {
        AddMovementInput(world_dir);
    }
    if (IsGrounded() && game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown))
    {
        Jump();   // 空中携行ロジック(旧175-179行)はCharacterが持つので不要
    }
    Character::Tick(dt);
}

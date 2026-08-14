#include "player.h"
#include "player_camera.h"
#include "../../Engine/Components/animation_component.h"
#include "../../Game/GameMain.h"

Player::Player() : Character("Assets/Mesh/remy.skmesh")
{
    // Character がロコモーションを選ぶため、CharaAnim の名前で登録する。
    // 末尾の真偽値はループ再生の有無。ジャンプと着地は1回きり再生する。
    animation_->AddAnimation(CharaAnim::kRun, AnimatorManager::Get().Load("Assets/Animation/run.anim"), true);
    animation_->AddAnimation(CharaAnim::kIdle, AnimatorManager::Get().Load("Assets/Animation/idle.anim"), true);
    animation_->AddAnimation(CharaAnim::kJump, AnimatorManager::Get().Load("Assets/Animation/jump_up.anim"), false);
    animation_->AddAnimation(CharaAnim::kFall, AnimatorManager::Get().Load("Assets/Animation/fall.anim"), true);
    animation_->AddAnimation(CharaAnim::kLanding, AnimatorManager::Get().Load("Assets/Animation/landing.anim"), false);

    animation_->Play(CharaAnim::kIdle);
    // remyのメッシュがcm単位で作られているため、ワールド単位へ縮める。
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
}

void Player::Begin()
{
    transform_.position = Vec3(0, 0, 0);
    use_tick_ = true;
    // カメラは同じフレームで生成される場合があり、ここで見つからないことがある。
    // そのときは Tick 側で取り直す。
    camera_ = GetWorld()->FindActor<PlayerCamera>();
    Character::Begin();
}

void Player::Tick(float dt)
{
    // WASDをカメラから見たローカルの移動方向として集計する。
    Vec3 local{};
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kDown)) local.z += 1.0f;
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kDown)) local.z -= 1.0f;
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown)) local.x += 1.0f;
    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown)) local.x -= 1.0f;

    // カメラのyawで回してワールド方向に直す。
    // 速度と実際の移動は Character が受け持つので、ここでは向きだけを渡す。
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
    // 空中での二段ジャンプ防止は Character::Jump が行うので、ここでは入力を渡すだけ。
    if (IsGrounded() && game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown))
    {
        Jump();
    }
    Character::Tick(dt);
}

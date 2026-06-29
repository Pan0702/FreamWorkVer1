#include "player.h"
#include <cmath>

#include "../../Debug/ImGui/imgui.h"
#include "../../Engine/Components/skeletal_mesh.h"
#include "../../Engine/Components/animation_component.h"
#include "../../Engine/Components/sphere_collider_componet.h"
#include "../../Game/GameMain.h"
#include "../../Core/Math/intersect.h"
#include "PlayerComponent/state_component.h"
#include "player_common.h"
#include "../../Engine/Components/capsule_collider_comp.h"

namespace
{
    inline const std::string kRun = "run";
    inline const std::string kIdle = "idle";
    inline const std::string kJump = "jump";
    inline const std::string kFall = "fall";
    inline const std::string kLanding = "landing";
}

Player::Player()
{
    SkeletalMesh* sk = SkeletalMeshManager::Get().Load("Assets/Mesh/remy.skmesh");
    materials_ = std::make_unique<MaterialSlot>(sk->GetMaterialDecs());
    AddComponent<SkeletalMeshComponent>(sk, materials_.get());
    animation_ = AddComponent<AnimationComponent>();
    
    animation_->AddAnimation(kRun, AnimatorManager::Get().Load("Assets/Animation/run.anim"), true);
    animation_->AddAnimation(kIdle, AnimatorManager::Get().Load("Assets/Animation/idle.anim"), true);
    animation_->AddAnimation(kJump, AnimatorManager::Get().Load("Assets/Animation/jump_up.anim"), false);
    animation_->AddAnimation(kFall, AnimatorManager::Get().Load("Assets/Animation/fall.anim"), true);
    animation_->AddAnimation(kLanding, AnimatorManager::Get().Load("Assets/Animation/landing.anim"), false);
    animation_name_ = kIdle;
    animation_->Play(kIdle);
    auto capsule = AddComponent<CapsuleColliderComponent>(sk);
    capsule->SetOnHit(this, &Player::OnHit);
    capsule->SetUseTransform(true);
    capsule->SetDraw(true);
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
    std::vector<PlayerState> states = {PlayerState::kJump, PlayerState::kWalk};
    states_ = factor_.GetComponents(states);
}

void Player::Begin()
{
    Actor::Begin();
}

void Player::Tick(float dt)
{
    // 足元から真下へレイを撃って、このフレームの重力処理前に接地状態を確定する。//
    {
        constexpr float kProbeMargin = 0.1f; // 接地とみなす許容ギャップ//
        Ray ray;
        constexpr float kProbeUp = 0.5f; 
        ray.origin = transform_.position + Vec3(0.0f, kProbeUp, 0.0f);
        ray.direction = Vec3(0.0f, -1.0f, 0.0f);
        ray.distance = kProbeUp + kProbeMargin;
        ContactInfo hit;
        is_grounded_ = GetWorld()->GetCollisionWorld().Raycast(ray, hit);
        if (is_grounded_)
        {
          vel_.y = ray.origin.y - (hit.normal * hit.depth).y;
        }
    }

    pl_input_ = Input(dt);
    SetState(BuildWantedState(pl_input_));
    // 有効な状態コンポーネントだけを更新して、入力を移動量へ反映する。//
    for (auto& [state, comp] : states_)
    {
        // 現在の状態ビットに含まれているコンポーネントだけ Tick する。//
        if (IsSet(state_bit_, state))
        {
            comp->Tick(dt, pl_input_);
        }
    }
    
    // ジャンプ入力がなく空中にいる間は、落下速度を加算して移動量に反映する。//
    if (!pl_input_.jump && !is_grounded_)
    {
        vel_.y -= kGravityUp * kFallMul * dt;
        pl_input_.move_amount += vel_ * dt;
    }
    else
    {
        // 接地中またはジャンプ処理中は、この落下用速度を持ち越さない。//
        vel_.y = 0.0f;
    }
    transform_.position += pl_input_.move_amount;
    transform_.rotation.y = pl_input_.yaw;
    
    const bool just_landed = is_grounded_ && !was_grounded_;
    const bool landing_playing = (animation_name_ == kLanding && animation_->IsPlaying());
    std::string anim_name;
    
    {
        // ジャンプ中はジャンプアニメーションを選ぶ。//
        if (pl_input_.jump)            anim_name = kJump;
        else if (!is_grounded_)        anim_name = kFall;
        else if (just_landed)          anim_name = kLanding;        // 着地に入る
        else if (landing_playing)      anim_name = kLanding;        // 終わるまで維持
        else if (pl_input_.move_amount.LengthSquared() > kEpsilon) anim_name = kRun;
        else                           anim_name = kIdle;
    }
    
    // 前フレームと違うアニメーションになった時だけ再生を切り替える。//
    if (anim_name != animation_name_ && !anim_name.empty())
    {
        animation_name_ = anim_name;
        animation_->CrossFade(anim_name,0.2f);       
    }
    was_grounded_ = is_grounded_;
    Actor::Tick(dt);
}

PlayerInput Player::Input(float dt)
{
    PlayerInput input;
    input.yaw = pl_input_.yaw;
    constexpr float move_speed = 10.0f;

    // A キー入力を左方向の移動量へ変換する。//
    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown))
    {
        input.move_amount.x -= move_speed * dt;
    }
    // D キー入力を右方向の移動量へ変換する。//
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown))
    {
        input.move_amount.x += move_speed * dt;
    }
    // W キー入力を前方向の移動量へ変換する。//
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kDown))
    {
        input.move_amount.z += move_speed * dt;
    }
    // S キー入力を後ろ方向の移動量へ変換する。//
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kDown))
    {
        input.move_amount.z -= move_speed * dt;
    }
    const bool space = game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown);
    // 接地中にスペースを押している場合だけ、新しくジャンプ入力を立てる。//
    if (is_grounded_ && space)
    {
        input.jump = true;        
    }
    // 接地中でスペースを押していなければ、ジャンプ状態を止める。//
    else if (is_grounded_)
    {
        input.jump = false;
    }
    else
    {
        // 空中では前フレームのジャンプ状態を引き継ぎ、ジャンプ中か落下中かを保つ。//
        input.jump = pl_input_.jump;
    }
    input.move_dir = input.move_amount;
    return input;
}

uint32 Player::BuildWantedState(const PlayerInput& in) const
{
    uint32 next_state = 0;
    // 移動方向がある場合は歩き状態、なければ待機状態を選ぶ。//
    if (in.move_dir.LengthSquared() > kEpsilon)
    {
        next_state |= static_cast<int>(PlayerState::kWalk);
    }
    else
    {
        next_state |= static_cast<int>(PlayerState::kIdle);
    }
    // ジャンプ入力が有効な場合はジャンプ状態も追加する。//
    if (in.jump)
    {
        next_state |= static_cast<int>(PlayerState::kJump);
    }
    return next_state;
}

void Player::SetState(uint32 new_bits)
{
    const uint32 enter_bits = new_bits & ~state_bit_;
    const uint32 exit_bits = ~new_bits & state_bit_;
    // 状態ビットの差分を見て、開始処理と終了処理を必要なコンポーネントだけに流す。//
    for (auto& [state, comp] : states_)
    {
        // 今回新しく有効になった状態なら OnEnter を呼ぶ。//
        if (enter_bits & Bit(state))
        {
            comp->OnEnter();
        }
        // 今回無効になった状態なら OnExit を呼ぶ。//
        if (exit_bits & Bit(state))
        {
            comp->OnExit();
        }
    }
    state_bit_ = new_bits;
}

void Player::OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info)
{
    // 押し出しのみ行い、接地判定は Tick 冒頭の下向きレイに任せる。//
    transform_.position += info.normal * info.depth;
}
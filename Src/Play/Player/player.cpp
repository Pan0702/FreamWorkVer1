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
    const std::string gangnam = "gangnam";
    const std::string run = "run";
    const std::string idle = "idle";
    const std::string jump = "jump";
}

Player::Player()
{
    SkeletalMesh* sk = SkeletalMeshManager::Get().Load("Assets/Mesh/remy.skmesh");
    materials_ = std::make_unique<MaterialSlot>(sk->GetMaterialDecs());
    AddComponent<SkeletalMeshComponent>(sk, materials_.get());
    animation_ = AddComponent<AnimationComponent>();

    animation_->AddAnimation(gangnam, AnimatorManager::Get().Load("Assets/Animation/gangnam.anim"), true);
    animation_->AddAnimation(run, AnimatorManager::Get().Load("Assets/Animation/run.anim"), true);
    animation_->AddAnimation(idle, AnimatorManager::Get().Load("Assets/Animation/idle.anim"), true);
    animation_->AddAnimation(jump, AnimatorManager::Get().Load("Assets/Animation/jump.anim"), false);
    animation_->Play(idle);
    auto capsule = AddComponent<CapsuleColliderComponent>(sk);
    Vec3 t = (sk->GetBounds().max - sk->GetBounds().min) * 0.5f;
    char buf[128];
    sprintf_s(buf, "half_size = %.3f, %.3f, %.3f\n", t.x, t.y, t.z);
    OutputDebugStringA(buf);
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
    // 足元から真下へレイを撃って接地判定する。Tick 冒頭で同期的に取るので
    // このフレームの重力判断にそのまま使える（1フレーム遅延しない）。
    {
        constexpr float kProbeRadius = 1.0f; // 体の球コライダー半径（ワールド）
        constexpr float kProbeMargin = 0.2f; // 接地とみなす許容ギャップ
        Ray ray;
        ray.origin = transform_.position;
        ray.direction = Vec3(0.0f, -1.0f, 0.0f);
        ray.distance = kProbeRadius + kProbeMargin;
        ContactInfo hit;
        is_grounded_ = GetWorld()->GetCollisionWorld().Raycast(ray, hit);
    }

    pl_input_ = Input(dt);
    SetState(BuildWantedState(pl_input_));
    for (auto& [state, comp] : states_)
    {
        if (IsSet(state_bit_, state))
        {
            comp->Tick(dt, pl_input_);
        }
    }
    if (!pl_input_.jump && !is_grounded_)
    {
        vel_.y -= kGravityUp * kFallMul * dt;
        pl_input_.move_amount += vel_ * dt;
    }
    else
    {
        vel_.y = 0.0f;
    }
    transform_.position += pl_input_.move_amount;
    transform_.rotation.y = pl_input_.yaw;


    Actor::Tick(dt);
}

PlayerInput Player::Input(float dt)
{
    PlayerInput input;
    input.move_dir.y = pl_input_.yaw;
    constexpr float move_speed = 10.0f;

    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown))
    {
        input.move_amount.x -= move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown))
    {
        input.move_amount.x += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kDown))
    {
        input.move_amount.z += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kDown))
    {
        input.move_amount.z -= move_speed * dt;
    }
    const bool space = game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown);
    if (is_grounded_ && space)
    {
        input.jump = true;        
    }
    else if (is_grounded_)
    {
        input.jump = false;
    }
    else
    {
        input.jump = pl_input_.jump;
    }

    return input;
}

uint32 Player::BuildWantedState(const PlayerInput& in) const
{
    uint32 next_state = 0;
    if (in.move_dir.LengthSquared() > 1e-6f)
    {
        next_state |= static_cast<int>(PlayerState::kWalk);
    }
    else
    {
        next_state |= static_cast<int>(PlayerState::kIdle);
    }
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
    for (auto& [state, comp] : states_)
    {
        if (enter_bits & Bit(state))
        {
            comp->OnEnter();
        }
        if (exit_bits & Bit(state))
        {
            comp->OnExit();
        }
    }
    state_bit_ = new_bits;
}

void Player::OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info)
{
    // 押し出しのみ。接地判定は Tick 冒頭の下向きレイが担当する。
    transform_.position += info.normal * info.depth;
}

#include "player.h"

#include "../../Debug/ImGui/imgui.h"
#include "../../Engine/Components/skeletal_mesh.h"
#include "../../Engine/Components/animation_component.h"
#include "../../Engine/Components/sphere_collider_componet.h"
#include "../../Game/GameMain.h"

#include <cmath>

#include "../../Core/Math/intersect.h"
#include "PlayerComponent/state_component.h"

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
    auto sphere = AddComponent<SphereColliderComponent>();
    sphere->SetOnHit(this, &Player::OnHit);
    sphere->SetRadius(1.0f);
    sphere->SetUseTransform(false);
    transform_.position = Vec3(0, 0, 0);
    transform_.rotation = Vec3(0, 0, 0);
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
}

void Player::Begin()
{
    Actor::Begin();
}

void Player::Tick(float dt)
{
    pl_input_ = Input(dt);

    if (state_bit_ & static_cast<int>(PlayerState::kJump) != 0)
    {
        States.at(PlayerState::kJump)->Tick(dt, pl_input_);
    }
    if (state_bit_ & static_cast<int>(PlayerState::kWalk) != 0)
    {
        States.at(PlayerState::kWalk)->Tick(dt, pl_input_);
    }
    if (state_bit_ & static_cast<int>(PlayerState::kIdle) != 0)
    {
        States.at(PlayerState::kIdle)->Tick(dt, pl_input_);
    }
    
    if (is_moving_)
    {
        transform_.position += pl_input_.move_dir;
        transform_.rotation = pl_input_.move_dir;
    }
    Actor::Tick(dt);
}

PlayerInput Player::Input(float dt)
{
    PlayerInput input;
    constexpr float move_speed = 10.0f;
    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown))
    {
        input.move_dir.x -= move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown))
    {
        input.move_dir.x += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kDown))
    {
        input.move_dir.z += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kDown))
    {
        input.move_dir.z -= move_speed * dt;
    }
    if (input.move_dir.LengthSquared() > 1e-6f)
    {
        state_bit_ = static_cast<int>(PlayerState::kWalk);
    }
    else
    {
        state_bit_ = static_cast<int>(PlayerState::kIdle);
    }


    if (game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown))
    {
        if (!pl_input_.jump)
        {
            state_bit_ = static_cast<int>(PlayerState::kJump);
            input.jump = true;
            States.at(PlayerState::kWalk)->OnEnter();
        }
    }

    input.move_dir = transform_.rotation;
    return input;
}

void Player::OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info)
{
    transform_.position += info.normal * info.depth;
    DEBUG_LOG("OnHit\n");
    state_bit_ = static_cast<int>(PlayerState::kIdle);
    pl_input_.jump = false;
}

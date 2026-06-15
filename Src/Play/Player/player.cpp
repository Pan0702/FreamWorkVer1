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

    constexpr float kApexHeight = 4.233f;
    constexpr float kApexTime = 0.373f;
    constexpr float kFallMul = 0.8f;

    constexpr float kJumpVel = 2.0f * kApexHeight / kApexTime;
    constexpr float kGravityUp = 2.0f * kApexHeight / (kApexTime * kApexTime);
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


    if (game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown) && is_grounded_)
    {
        vel_.y = kJumpVel;
        is_grounded_ = false;
        //animation_->CrossFade(jump, 0.2f);
    }

    if (!is_grounded_)
    {
        velocity += ClalcMovingJumoAmount(dt);
        is_moving = true;
    }

    if (is_moving != is_moving_)
    {
        is_moving_ = is_moving;
        animation_->CrossFade(is_moving_ ? run : idle, 0.2f);
    }
    if (is_moving_)
    {
        transform_.position += velocity;
    }
    ImGui::Begin("test");
    ImGui::SliderFloat3("position", &transform_.position.x, -300.0f, 300.0f);
    ImGui::Text("position: (%.2f, %.2f, %.2f)", transform_.position.x, transform_.position.y, transform_.position.z);
    ImGui::End();
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
    input.jump = game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown);
    return input;
}

Vec3 Player::ClalcMovingAmount(float dt)
{
    // ˆÚ“®‘¬“x //
    Vec3 velocity;

    return velocity;
}

Vec3 Player::ClalcMovingJumoAmount(float dt)
{
    Vec3 velocity;
    float g = (vel_.y > 0.0f) ? kGravityUp : kGravityUp * kFallMul;
    vel_.y -= g * dt;
    velocity.y = vel_.y * dt;
    return velocity;
}

float Player::ClalcRotationAmount(const Vec3& velocity)
{
    if (velocity.LengthSquared() <= 1e-6f)
    {
        return 0.0f;
    }


}

void Player::OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info)
{
    transform_.position += info.normal * info.depth;
    DEBUG_LOG("OnHit\n");
    is_grounded_ = true;
}

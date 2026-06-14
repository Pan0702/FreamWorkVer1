#include "player.h"

#include "../../Debug/ImGui/imgui.h"
#include "../../Engine/Components/skeletal_mesh.h"
#include "../../Engine/Components/animation_component.h"
#include "../../Engine/Components/sphere_collider_componet.h"
#include "../../Game/GameMain.h"

#include <cmath>

namespace
{
    const std::string gangnam = "gangnam";
    const std::string run = "run";
    const std::string idle = "idle";
    const std::string jump = "jump";
    
    constexpr float kApexHeight = 2.0f;  
    constexpr float kApexTime   = 0.32f;  
    constexpr float kFallMul    = 1.8f;   
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
    Vec3 velocity = ClalcMovingAmount(dt);
    bool is_moving = velocity.LengthSquared() > 1e-6f;

    if (is_moving)
    {
        const float delta = ClalcRotationAmount(velocity);
        constexpr float rot_speed = 10.0f;
        const float step = rot_speed * dt;

        if (std::abs(delta) <= step)
        {
            transform_.rotation.y = NormalizeAngleRad(transform_.rotation.y + delta);
        }
        else
        {
            transform_.rotation.y += delta > 0.0f ? step : -step;
            transform_.rotation.y = NormalizeAngleRad(transform_.rotation.y);
        }
    }

    if (game_main->GetInput().CheckKey(InputKey::kSpace, KeyState::kDown) && is_grounded_)
    {
        const float jump_vel   = 2.0f * kApexHeight / kApexTime;
        vel_.y = jump_vel;
        is_grounded_ = false;
        jump_timer_ = 0.0f;
        //animation_->CrossFade(jump, 0.2f);
    }

    if (!is_grounded_)
    {
        velocity += ClalcMovingJumoAmount(dt);
        is_moving = true;
    }
    if (transform_.position.y < 0.0f)
    {
        transform_.position.y = 0.0f;
        is_grounded_ = true;
        is_moving = false;
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
    ImGui::Begin("jump");

    ImGui::SliderFloat("kGravity", &kGravity, 0.0f, 100.0f);
    ImGui::SliderFloat("kJumpHight", &kJumpHight, 0.0f, 6.0f);
    ImGui::End();
    Actor::Tick(dt);
}

Vec3 Player::ClalcMovingAmount(float dt)
{
    constexpr float move_speed = 10.0f; // ˆÚ“®‘¬“x //
    Vec3 velocity;
    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown))
    {
        velocity.x -= move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown))
    {
        velocity.x += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kDown))
    {
        velocity.z += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kDown))
    {
        velocity.z -= move_speed * dt;
    }
    return velocity;
}

Vec3 Player::ClalcMovingJumoAmount(float dt)
{

    const float gravity_up = 2.0f * kApexHeight / (kApexTime * kApexTime);
    Vec3 velocity;
    float g = (vel_.y > 0.0f) ? gravity_up : gravity_up * kFallMul;
    vel_.y -= g * dt;         
    velocity.y =  vel_.y * dt;  
    return velocity;
}

float Player::ClalcRotationAmount(const Vec3& velocity)
{
    if (velocity.LengthSquared() <= 1e-6f)
    {
        return 0.0f;
    }

    const float current_yaw = transform_.rotation.y;
    const float target_yaw = std::atan2f(velocity.x, velocity.z);
    return NormalizeAngleRadSigned(target_yaw - current_yaw);
}

void Player::OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info)
{
}

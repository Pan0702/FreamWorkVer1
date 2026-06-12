#include "player.h"

#include "../../Debug/ImGui/imgui.h"
#include "../../Engine/Components/skeletal_mesh.h"
#include "../../Engine/Components/animation_component.h"
#include "../../Engine/Components/sphere_collider_componet.h"
#include "../../Game/GameMain.h"

namespace 
{
    const std::string gangnam = "gangnam";
    const std::string run = "run";
    const std::string idle = "idle";
}
Player::Player()
{
    SkeletalMesh* sk = SkeletalMeshManager::Get().Load("Assets/Mesh/remy.skmesh");
    materials_ = std::make_unique<MaterialSlot>(sk->GetMaterialDecs());
    AddComponent<SkeletalMeshComponent>(sk, materials_.get());
    
     animation_ = AddComponent<AnimationComponent>();

    animation_->AddAnimation(gangnam,AnimatorManager::Get().Load("Assets/Animation/gangnam.anim"),true);
    animation_->AddAnimation(run,AnimatorManager::Get().Load("Assets/Animation/run.anim"),true);
    animation_->AddAnimation(idle,AnimatorManager::Get().Load("Assets/Animation/idle.anim"),true);
    animation_->Play(idle);
    auto sphere = AddComponent<SphereColliderComponent>();
    sphere->SetOnHit(this,&Player::OnHit);
    transform_.position = Vec3(0, 0, 0);
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
}

void Player::Begin()
{
    Actor::Begin();

}

void Player::Tick(float dt)
{
    
    constexpr float move_speed = 5.0f;     // 移動速度 //
    static bool is_moving = false;         // 動いてるかどうか //
    Vec3 move_amount;
    if (game_main->GetInput().CheckKey(InputKey::kA,KeyState::kDown))
    {
        move_amount.x -= move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kD,KeyState::kDown))
    {
        move_amount.x += move_speed* dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kW,KeyState::kDown))
    {
        move_amount.z += move_speed * dt;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS,KeyState::kDown))
    {
        move_amount.z -= move_speed * dt;
    }
    
    bool tmp = std::abs(move_amount.LengthSquared()) > 1e-6f;

    if (tmp != is_moving)
    {
        is_moving = tmp;
        animation_->CrossFade(is_moving ? run : idle,0.2f);
    }
    if (is_moving)
    {
        transform_.position += move_amount;
    }
    
    ImGui::Begin("Player");
    ImGui::Text("%lf,%lf,%lf", transform_.position.x, transform_.position.y, transform_.position.z);
    ImGui::Text("lngthsquared: %lf", move_amount.LengthSquared());
    ImGui::End();
    Actor::Tick(dt);

}

void Player::OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info)
{
    
}

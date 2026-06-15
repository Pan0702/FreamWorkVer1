#pragma once
#include "../../Engine/actor.h"
#include "../../Resource/material_slot.h"
class StateComponentBase;
enum class PlayerState : uint8_t
{
    kIdle = 1 << 0,
    kWalk = 1 << 1,
    kJump = 1 << 2,
};

class Player : public Actor
{
public:
    Player();

private:
    void Begin() override;
    void Tick(float dt) override;
    Vec3 ClalcMovingAmount(float dt);
    Vec3 ClalcMovingJumoAmount(float dt);
    float ClalcRotationAmount(const Vec3& velocity);
    void OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info);
    std::unique_ptr<MaterialSlot> materials_;
    class AnimationComponent* animation_ = nullptr;

    Vec3 vel_;
    bool is_moving_ = false;
    bool is_grounded_ = true;
    
    std::unordered_map<PlayerState,StateComponentBase*> States;
};

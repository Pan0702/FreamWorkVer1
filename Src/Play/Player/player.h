#pragma once
#include "../../Engine/actor.h"
#include "../../Resource/material_slot.h"
#include "PlayerComponent/component_factor.h"
class StateComponentBase;
enum class PlayerState : uint8_t
{
    kIdle = 1 << 0,
    kWalk = 1 << 1,
    kJump = 1 << 2,
};

struct PlayerInput
{
    Vec3 move_amount = {0.0f, 0.0f, 0.0f};
    Vec3 move_dir = {0.0f, 0.0f, 0.0f};
    bool jump = false;
};


class Player : public Actor
{
public:
    Player();

private:
    void Begin() override;
    void Tick(float dt) override;
    PlayerInput Input(float dt);
    void OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info);
    std::unique_ptr<MaterialSlot> materials_;
    class AnimationComponent* animation_ = nullptr;

    Vec3 vel_;
    uint32_t state_bit_ = 0;
    bool is_moving_ = false;
    bool is_grounded_ = true;
    PlayerInput pl_input_;
    ComponentFactor factor_;
    std::unordered_map<PlayerState,StateComponentBase*> states_;
};

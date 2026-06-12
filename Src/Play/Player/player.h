#pragma once
#include "../../Engine/actor.h"
#include "../../Resource/material_slot.h"

class Player : public Actor
{
public:
    Player();
    
private:
    void Begin() override;
    void Tick(float dt) override;
    void OnHit(ColliderComponent* self, Actor* other_actor,ColliderComponent* other_coll, const ContactInfo& info);
    std::unique_ptr<MaterialSlot> materials_;
    class AnimationComponent* animation_ = nullptr;
    
};

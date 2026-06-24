#pragma once
#include "../../Engine/actor.h"

class GoalFlag : public Actor
{
public:
    GoalFlag();
private:
    std::unique_ptr<MaterialSlot> materials_;
    void OnBeginOverlap(const ColliderComponent* self, const ColliderComponent* other);
};

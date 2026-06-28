#pragma once
#include "../../Engine/actor.h"

class GoalFlag : public Actor
{
public:
    GoalFlag();

    void OnBeginOverlap(const ColliderComponent* c1,const ColliderComponent* c2);
private:
    std::unique_ptr<MaterialSlot> materials_;
};

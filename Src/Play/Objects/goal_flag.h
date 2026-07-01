#pragma once
#include "../../Engine/actor.h"

class GoalFlag : public Actor
{
public:
    GoalFlag();
    GoalFlag(const Vec3& pos);
    void OnBeginOverlap(const ColliderComponent* c1,const ColliderComponent* c2);
private:
    std::unique_ptr<MaterialSlot> materials_;

};

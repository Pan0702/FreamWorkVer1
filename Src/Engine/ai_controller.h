#pragma once
#include "actor.h"
#include "controller.h"

class AiController : public Controller
{
public:
    void MoveToActor(Actor* actor);
    void StopMovement();

private:
    void Tick(float dt) override;

    Actor* target_ = nullptr;
    class NavigationAgentComponent* nav_agent_ = nullptr;
    Vec3 last_goal_;
    float repath_timer_ = 0.0f;
    bool has_goal_ = false;
};

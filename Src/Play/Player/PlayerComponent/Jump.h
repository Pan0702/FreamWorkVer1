#pragma once
#include "state_component.h"

class PlayerJump : public StateComponentBase
{
public:
    void OnEnter() override;
    void Tick(float dt, PlayerInput& input) override;
    void OnExit() override;

private:
    float vel_y_;
};

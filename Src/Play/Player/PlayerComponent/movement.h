#pragma once
#include "state_component.h"

class PlayerMovement : public StateComponentBase
{
public:
    PlayerMovement() = default;
    ~PlayerMovement();
    void OnEnter() override;
    void Tick(float dt,PlayerInput& input) override;
    void OnExit() override;
};

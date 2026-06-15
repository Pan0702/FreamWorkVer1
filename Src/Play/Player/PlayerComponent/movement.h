#pragma once
#include "state_component.h"

class PlayerMovement : public StateComponentBase
{
public:
    void OnEnter() override;
    void Tick(float dt,const PlayerInput& input) override;
    void OnExit() override;
};

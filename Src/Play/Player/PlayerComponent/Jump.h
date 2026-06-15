#pragma once
#include "state_component.h"

class PlayerJump : public StateComponentBase
{
public:
    void OnEnter() override;
    void Tick(float dt) override;
    void OnExit() override;
};

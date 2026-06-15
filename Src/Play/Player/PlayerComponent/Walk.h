#pragma once
#include "state_component.h"

class PlayerWalk : public StateComponentBase
{
public:
    void OnEnter() override;
    void Tick(float dt) override;
    void OnExit() override;
};

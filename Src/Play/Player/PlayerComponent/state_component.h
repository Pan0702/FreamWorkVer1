#pragma once

class StateComponentBase
{
public:
    StateComponentBase() = default;
    virtual void OnEnter() = 0;
    virtual void Tick(float dt) = 0;
    virtual void OnExit() = 0; 
};

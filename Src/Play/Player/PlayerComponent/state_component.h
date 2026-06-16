#pragma once
struct PlayerInput;
class Player;

class StateComponentBase
{
public:
    virtual ~StateComponentBase() = default;
    virtual void OnEnter() = 0;
    virtual void Tick(float dt, PlayerInput& input) = 0;
    virtual void OnExit() = 0;

protected:
};

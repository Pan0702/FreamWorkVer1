#pragma once
struct PlayerInput;
class Player;

class StateComponentBase
{
public:
    StateComponentBase(Player& player) : player_(player){}
    virtual ~StateComponentBase() = default;
    virtual void OnEnter() = 0;
    virtual void Tick(float dt,const PlayerInput& input) = 0;
    virtual void OnExit() = 0; 
protected:
    Player& player_;
};

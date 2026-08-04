#pragma once
#include "actor.h"
class Character;
class Controller : public Actor
{
public:
    void Possess(Character* character);  
    void UnPossess();
    Character* GetCharacter() const;
private:
    Character* possessed_ = nullptr;
};
#include "controller.h"

void Controller::Possess(Character* character)
{
    possessed_ = character;
}

void Controller::UnPossess()
{
    possessed_ = nullptr;
}

Character* Controller::GetCharacter() const
{
    return possessed_;
}
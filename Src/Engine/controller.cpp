#include "controller.h"

void Controller::Possess(Character* character)
{
    // Š—LŒ ‚Í‚½‚È‚¢BCharacter ‚Ìõ–½‚Í World ‚ªŠÇ—‚·‚éB
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

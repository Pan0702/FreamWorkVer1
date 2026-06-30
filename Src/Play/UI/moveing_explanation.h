#pragma once
#include "../../Engine/actor.h"

class MoveingExplanation : public Actor
{
public:
    void Begin() override;
private:
    class SpriteComponent* explanation_ = nullptr;
};

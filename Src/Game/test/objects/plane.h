#pragma once
#include "../../../Engine/actor.h"

class Plane : public Actor
{
public:
    Plane();
private:
    std::unique_ptr<MaterialSlot> materials_;
};

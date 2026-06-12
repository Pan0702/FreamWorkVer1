#pragma once
#include <memory>

#include "../../Engine/actor.h"

class MaterialSlot;

class Cube : public Actor
{
public:
    Cube();
    
private:
    std::unique_ptr<MaterialSlot> materials_;
};

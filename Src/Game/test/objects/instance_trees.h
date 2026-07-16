#pragma once
#include <memory>

#include "../../../Engine/actor.h"

class InstanceTrees : public  Actor
{
public:

    InstanceTrees();

private:
    std::unique_ptr<MaterialSlot> slot_;
};

#pragma once
#include "../../../Engine/actor.h"
class Slope:public Actor
{
public:
    Slope();
    Slope(const Vec3& position);
private:
    std::unique_ptr<MaterialSlot> materials_;
};

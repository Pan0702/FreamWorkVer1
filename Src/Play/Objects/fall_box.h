#pragma once
#include "../../Engine/actor.h"

class FallBox :public Actor
{
public:
    FallBox();
    void OnBeginOverlap(const ColliderComponent* my,const ColliderComponent* other);
};

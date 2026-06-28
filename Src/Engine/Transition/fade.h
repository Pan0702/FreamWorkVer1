#pragma once
#include "transition_effect.h"
#include "../../Debug/debug.h"

class Fade : public ITransitionEffect
{
public:
    void Draw(float progress) override
    {
        Debug::Get().DrawBox(Vec2(0,0),Vec2(kWindowWidth,kWindwoHeight),
            Vec4(0,0,0,progress),0.0f);
    }
};


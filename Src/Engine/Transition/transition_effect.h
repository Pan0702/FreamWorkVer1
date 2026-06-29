#pragma once

class ITransitionEffect
{
public:
    virtual ~ITransitionEffect() = default;
    virtual void Draw(float progress) = 0;
};

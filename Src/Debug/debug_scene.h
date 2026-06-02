#pragma once
#include "../Core/Math/my_math.h"
#include "../Engine/component.h"
class Texture2D;
class DebugComponent : public Component
{
public:
    void Tick(float dt) override;
    void Render();
    
private:
    float time_ = 0.0f;
    
};

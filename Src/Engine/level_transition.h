#pragma once
#include <memory>
#include "../Core/Math/my_math.h"
#include "Transition/fade.h"
enum class TransitionPhase : uint8
{
    kNone,
    kIn,
    kOut,
};

class LevelTransition
{
public:
    void SetEffect(std::unique_ptr<ITransitionEffect> effect);
    void Start();
    void Tick(float dt);
    void Draw();
    bool GetJustCovered();
    bool IsPlaying();
private:
    float time_ = 0.0f; 
    float progress_ = 0.0f;
    const float duration_ = 0.4f;
    TransitionPhase phase_ = TransitionPhase::kNone;
    std::unique_ptr<ITransitionEffect> effect_;
    bool just_covered_ = false;
};

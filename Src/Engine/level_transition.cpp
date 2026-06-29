#include "level_transition.h"

#include <algorithm>

void LevelTransition::SetEffect(std::unique_ptr<ITransitionEffect> effect)
{
    effect_ = std::move(effect);
}

void LevelTransition::Start()
{
    phase_ = TransitionPhase::kOut;
    time_ = 0.0f;
    progress_ = 0.0f;
}

void LevelTransition::Tick(float dt)
{
    just_covered_ = false;
    if (phase_ == TransitionPhase::kOut)
    {
        time_ += dt;
        progress_ = std::clamp(time_ / duration_, 0.0f, 1.0f);
        if (progress_ >= 1.0f)
        {
            progress_ = 1.0f;
            just_covered_ = true;        
            phase_ = TransitionPhase::kIn;
            time_ = 0.0f;
        }
    }
    else if (phase_ == TransitionPhase::kIn)
    {
        time_ += dt;
        progress_ = 1.0f - std::clamp(time_ / duration_, 0.0f, 1.0f);
        if (time_ >= duration_)
        {
            progress_ = 0.0f;
            phase_ = TransitionPhase::kNone;
        }
    }
}

void LevelTransition::Draw()
{
    if (phase_ == TransitionPhase::kNone || !effect_) return;
    effect_->Draw(progress_);
}

bool LevelTransition::GetJustCovered()
{
    return just_covered_;
}

bool LevelTransition::IsPlaying()
{
    return phase_ != TransitionPhase::kNone;
}

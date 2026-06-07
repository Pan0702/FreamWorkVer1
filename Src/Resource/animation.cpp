#include "animation.h"

void Animation::SetData(float duration, float ticks_per_second,  std::vector<NodeAnimation> node)
{
    duration_ = duration;
    ticks_per_second_ = ticks_per_second;
    node_animations_ = std::move(node);
}

float Animation::GetDuration() const
{
    return duration_;
}

float Animation::GetTicksPerSecond() const
{
    return ticks_per_second_;
}

const std::vector<NodeAnimation>& Animation::GetNodeAnimations() const
{
    return node_animations_;
}

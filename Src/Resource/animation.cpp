#include "animation.h"

void Animation::SetData(float duration, float ticks_per_second,  std::vector<NodeAnimation> node)
{
    duration_ = duration;
    ticks_per_second_ = ticks_per_second;
    node_animations_ = std::move(node);
}

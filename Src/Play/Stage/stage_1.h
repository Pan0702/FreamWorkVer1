#pragma once
#include "../../Engine/level_base.h"
// PlayLevel ‚ÉŠÖŒW‚·‚éó‘Ô‚ÆU‚é•‘‚¢‚ğ‚Ü‚Æ‚ß‚éŒ^B
class Stage1 : public LevelBase
{
public:
     Stage1() = default;
    ~ Stage1() override = default;
    /**
     * @brief ¶¬‚Ü‚½‚Í‘JˆÚ’¼Œã‚É•K—v‚È‰Šúˆ—‚ğs‚¤B
     */
    void OnEnter() override;
};

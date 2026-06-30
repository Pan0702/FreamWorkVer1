#pragma once
#include "../../Engine/level_base.h"
// PlayLevel ‚ÉŠÖŒW‚·‚éó‘Ô‚ÆU‚é•‘‚¢‚ğ‚Ü‚Æ‚ß‚éŒ^B
class PlayLevel : public LevelBase
{
public:
    PlayLevel() = default;
    ~PlayLevel() override = default;
    /**
     * @brief ¶¬‚Ü‚½‚Í‘JˆÚ’¼Œã‚É•K—v‚È‰Šúˆ—‚ğs‚¤B
     */
    void OnEnter() override;
};

#pragma once
#include "../Engine/level_base.h"
class PlayLevel : public LevelBase
{
public:
    PlayLevel();
    void OnEnter() override;
};

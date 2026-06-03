#pragma once
#include "level_manager.h"
#include "world.h"

class GameInstance
{
public:
    bool Initialize();
    void Tick(float dt);
    void Render();

    World* GetWorld();
    LevelManager& GetLevelManager();

private:
    World world_;
    LevelManager level_manager_;
};

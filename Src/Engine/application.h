#pragma once
#include "../Core/common.h"
#include "Actor.h"
#include "World.h"

class Application
{
public:
    bool Initialize();
    void Tick(float dt);
    void Render();

private:

    std::unique_ptr<World> world_;
    Actor* test_actor_ = nullptr;
};

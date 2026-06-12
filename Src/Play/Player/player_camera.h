#pragma once
#include "../../Game/GameMain.h"

class PlayerCamera : public Actor
{
private:
    void Begin() override;
    void Tick(float dt) override;
    Camera* camera_ = nullptr;
    class Player* player_ = nullptr;
};

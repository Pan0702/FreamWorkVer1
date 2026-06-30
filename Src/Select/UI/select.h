#pragma once
#include "../../Engine/actor.h"
class SpriteComponent;
class Select : public Actor
{
public:
    Select();
    void Begin() override;
    void Tick(float dt) override;
private:
    void Input();
    int button_index_ = 0;
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* cur_texture_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
};

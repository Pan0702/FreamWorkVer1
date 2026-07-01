#pragma once
#include "../../Engine/actor.h"

class SpriteComponent;

class Title : public Actor
{
public:
    Title();
    void Tick(float dt) override;
private:
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
};

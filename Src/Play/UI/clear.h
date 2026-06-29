#pragma once
#include "../../Engine/actor.h"

class SpriteComponent;

class Clear : public Actor
{
public:
    Clear();
    void Begin() override;
    void Tick(float dt) override;
    
private:
    void Input() const;
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* cur_texture_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
};

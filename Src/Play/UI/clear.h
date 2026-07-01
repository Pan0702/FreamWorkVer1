#pragma once
#include "../../Engine/actor.h"

class SpriteComponent;

enum class ClearButton : uint8
{
    kRestart = 0,
    kSelect,
};
class Clear : public Actor
{
public:
    Clear();
    void Begin() override;
    void Tick(float dt) override;
    void SetVisible(bool visible);
private:
    void Input() ;
    SpriteComponent* ui_ = nullptr;
    SpriteComponent* cur_texture_ = nullptr;
    SpriteComponent* overlay_ = nullptr;
    int button_index_ = 0;
    bool visible_ = false;
};

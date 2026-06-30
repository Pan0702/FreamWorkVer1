#pragma once
#include "../../Engine/actor.h"

class SpriteComponent;
class Option : public Actor
{
public:
    Option();
    ~Option() override = default;
    void Tick(float dt) override;
    bool IsVisible() const;
    void SetVisible(bool visible);
    static Option& Get();
private:
    void Input() const;
    SpriteComponent*ui_ = nullptr;
    SpriteComponent*cur_texture_ = nullptr;
    SpriteComponent*overlay_ = nullptr;
    bool visible_ = false;
};

#pragma once
#include "../../Engine/game_instance.h"

class SpriteComponent;
class Option : public GameInstance
{
public:
    Option();
    ~Option() = default;
    void Tick(float dt) override;
    bool IsVisible() const;
    void SetVisible(bool visible);
    
private:
    void Input() const;
    std::unique_ptr<SpriteComponent> ui_ = nullptr;
    std::unique_ptr<SpriteComponent> cur_texture_ = nullptr;
    std::unique_ptr<SpriteComponent> overlay_ = nullptr;
    bool visible_ = false;
};

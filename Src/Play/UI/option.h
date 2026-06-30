#pragma once
#include "../../Engine/actor.h"
#include "../../Engine/game_instance.h"

class SpriteComponent;
class Option : public GameInstance
{
public:
    Option();
    ~Option() override;
    void OnAttach(const AttachContext& context) const;
    void OnDetach() const;
    void Tick(float dt) override;
    bool IsVisible() const;
    void SetVisible(bool visible);
    static Option& Get();
private:
    void Input() const;
    std::unique_ptr<SpriteComponent> ui_ = nullptr;
    std::unique_ptr<SpriteComponent> cur_texture_ = nullptr;
    std::unique_ptr<SpriteComponent> overlay_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
    bool visible_ = false;
};

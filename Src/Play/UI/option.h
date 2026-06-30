#pragma once
#include "../../Engine/actor.h"
#include "../../Engine/game_instance.h"

enum class OptionButton : uint8
{
    kRestart = 0,
    kSelect,
    kExit,
};
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

private:
    void Input();
    void SetOpen();
    std::unique_ptr<SpriteComponent> ui_ = nullptr;
    std::unique_ptr<SpriteComponent> cur_texture_ = nullptr;
    std::unique_ptr<SpriteComponent> overlay_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
    int button_index_ = 0;
    bool visible_ = false;
};

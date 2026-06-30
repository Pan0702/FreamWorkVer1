#include "option.h"

#include <algorithm>

#include "../../Engine/Components/sprite_component.h"
#include "../../Game/GameMain.h"

namespace
{
    constexpr float kCursorX = 740.0f;
    constexpr float kCursorStartY = 514.0f;
    constexpr float kCursorStepY = 134.0f;

    constexpr int kMinSelectionIndex = 0;
    constexpr int kMaxSelectionIndex = 2;

    constexpr float kCursorWidth = 466.0f;
    constexpr float kCursorHeight = 125.0f;

    const Vec4 kOverlayColor = Vec4(0.8f, 0.8f, 0.8f, 0.7f);
}

Option::Option()
{
    Texture2D* tex = TextureManager::Get().Load(L"Assets/Texture/option.png");
    ui_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    ui_->SetSize(kWindowWidth, kWindwoHeight);
    ui_->sort_key = 1;

    overlay_ = AddComponent<SpriteComponent>(nullptr, SpriteSpace::kScreen);
    overlay_->color = kOverlayColor;
    overlay_->SetSize(kWindowWidth, kWindwoHeight);
    overlay_->sort_key = 0;

    Texture2D* tex_sele = TextureManager::Get().Load(L"Assets/Texture/select.png");
    cur_texture_ = AddComponent<SpriteComponent>(tex_sele, SpriteSpace::kScreen);
    cur_texture_->SetSize(kCursorWidth, kCursorHeight);
    cur_texture_->SetPos(kCursorX, kCursorStartY);
    cur_texture_->sort_key = 2;
}


void Option::Tick(float dt)
{
    if (game_main->GetInput().CheckKey(InputKey::kEsc, KeyState::kPressed))
    {
        SetVisible(!visible_);
    }
    ui_->SetVisible(visible_);
    overlay_->SetVisible(visible_);
    cur_texture_->SetVisible(visible_);
    if (!visible_)
    {
        return;
    }
    Input();
    Actor::Tick(dt);
}


void Option::Input() const
{
    static int t = 0;
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kPressed))
    {
        t--;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kPressed))
    {
        t++;
    }
    t = std::clamp(t, kMinSelectionIndex, kMaxSelectionIndex);
    cur_texture_->SetPos(kCursorX, kCursorStartY + t * kCursorStepY);
}

bool Option::IsVisible() const
{
    return visible_;
}

void Option::SetVisible(bool visible)
{
    visible_ = visible;
}

Option& Option::Get()
{
    static Option instance;
    return instance;
}

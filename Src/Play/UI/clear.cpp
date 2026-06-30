#include "clear.h"

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
Clear::Clear()
{
    Texture2D* tex = TextureManager::Get().Load(L"Assets/Texture/clear.png");
    ui_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    ui_->SetSize(kWindowWidth, kWindwoHeight);
    ui_->sort_key = 1;

    overlay_ = AddComponent<SpriteComponent>(nullptr, SpriteSpace::kScreen);
    overlay_->color = kOverlayColor;
    overlay_->SetSize(kWindowWidth, kWindwoHeight);
    overlay_->sort_key = 0;

    tex = TextureManager::Get().Load(L"Assets/Texture/select.png");
    cur_texture_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    cur_texture_->SetSize(kCursorWidth, kCursorHeight);
    cur_texture_->SetPos(740,514);
    cur_texture_->sort_key = 2;
    SetVisible(visible_);
}

void Clear::Begin()
{
    SetVisible(false);
    Actor::Begin();
}

void Clear::Tick(float dt)
{
    Input();
    Actor::Tick(dt);
}

void Clear::SetVisible(bool visible)
{
    visible_ = visible;
    overlay_->SetVisible(visible);
    ui_->SetVisible(visible);
    cur_texture_->SetVisible(visible);
}

void Clear::Input() 
{
    auto input = game_main->GetInput();
    if (input.CheckKey(InputKey::kW, KeyState::kPressed))
    {
         button_index_--;
    }
    if (input.CheckKey(InputKey::kS, KeyState::kPressed))
    {
        button_index_++;
    }
    button_index_ = std::clamp(button_index_,kMinSelectionIndex,kMaxSelectionIndex);
    cur_texture_->SetPos(kCursorX,kCursorStartY + static_cast<float>(button_index_) * kCursorStepY);
    
    if (input.CheckKey(InputKey::kEnter,KeyState::kPressed))
    {
        ClearButton b = static_cast<ClearButton>(button_index_);
        switch (b)
        {
        case ClearButton::kRestart:
            {
                const std::string name = game_main->GetGameInstance().GetLevelManager().GetCurrentLevelName();
                game_main->GetGameInstance().GetLevelManager().OpenLevel(name);
                break;
            }
        case ClearButton::kSelect:
            game_main->GetGameInstance().GetLevelManager().OpenLevel(LevelName::kSelect);
            break;
        default:
            break;
        }
    }
}

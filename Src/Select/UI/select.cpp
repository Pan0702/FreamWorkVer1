#include "select.h"

#include <algorithm>

#include "../../Engine/Components/sprite_component.h"
#include "../../Game/GameMain.h"

namespace
{
    const Vec2 kStartPos = Vec2(340.0f, 336.0f);
    const Vec2 kCursorStepY = Vec2(208.0f,366.0f);

    constexpr int kMinSelectionIndex = 0;
    constexpr int kMaxSelectionIndex = 5;

    constexpr float kCursorWidth = 284.0f;
    constexpr float kCursorHeight = 284.0f;

    const Vec4 kOverlayColor = Vec4(0.8f, 0.8f, 0.8f, 0.7f);
}
Select::Select()
{
    Texture2D* tex = TextureManager::Get().Load(L"Assets/Texture/selectlevel.png");
    ui_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    ui_->SetSize(kWindowWidth, kWindwoHeight);
    ui_->sort_key = 1;

    overlay_ = AddComponent<SpriteComponent>(nullptr, SpriteSpace::kScreen);
    overlay_->color = kOverlayColor;
    overlay_->SetSize(kWindowWidth, kWindwoHeight);
    overlay_->sort_key = 0;

    tex = TextureManager::Get().Load(L"Assets/Texture/selectoutline.png");
    cur_texture_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    cur_texture_->SetSize(kCursorWidth, kCursorHeight);
    cur_texture_->SetPos(kStartPos.x,kStartPos.y);
    cur_texture_->sort_key = 2;
}

void Select::Begin()
{
    Actor::Begin();
}

void Select::Tick(float dt)
{
    Input();
    int w = (button_index_ % 3);
    w *= static_cast<int>(kCursorStepY.x + kCursorWidth);
    int h = (button_index_ / 3);
    h *= static_cast<int>(kCursorStepY.y);
    cur_texture_->SetPos(kStartPos.x + static_cast<float>(w), kStartPos.y + static_cast<float>(h));
    Actor::Tick(dt);
}

void Select::Input()
{
    auto input = game_main->GetInput();
    if (input.CheckKey(InputKey::kW, KeyState::kPressed))
    {
        button_index_ -= 3;
    }
    if (input.CheckKey(InputKey::kS, KeyState::kPressed))
    {
        button_index_ += 3;
    }
    if (input.CheckKey(InputKey::kA, KeyState::kPressed))
    {
        button_index_--;
    }
    if (input.CheckKey(InputKey::kD, KeyState::kPressed))
    {
        button_index_++;
    }
    button_index_ = std::clamp(button_index_, kMinSelectionIndex, kMaxSelectionIndex);
    if (input.CheckKey(InputKey::kEnter, KeyState::kPressed))
    {
        switch (button_index_)
        {
            case 0:
            game_main->GetGameInstance().GetLevelManager().OpenLevel(LevelName::kPlay);
            break;
            case 1:
            game_main->GetGameInstance().GetLevelManager().OpenLevel("Play");
            break;
            case 2:
            game_main->GetGameInstance().GetLevelManager().OpenLevel("Select");
            break;
            case 3:
            game_main->GetGameInstance().GetLevelManager().OpenLevel("Test");
            break;
            default:
            break;
        }
    }
}

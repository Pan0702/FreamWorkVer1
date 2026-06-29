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
}

void Clear::Begin()
{
    Actor::Begin();
}

void Clear::Tick(float dt)
{
    Input();
    Actor::Tick(dt);
}

void Clear::Input() const
{
    static int t  = 0;
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kPressed))
    {
        t--;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kPressed))
    {
        t++;
    }
    t = std::clamp(t,kMinSelectionIndex,kMaxSelectionIndex);
    cur_texture_->SetPos(kCursorX,kCursorStartY + t * kCursorStepY);
}

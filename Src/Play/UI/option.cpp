#include "option.h"

#include <algorithm>

#include "../../Engine/Components/sprite_component.h"
#include "../../Game/GameMain.h"
#include "../../Renderer/ui_renderer.h"

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
    ui_ = std::make_unique<SpriteComponent>(tex, SpriteSpace::kScreen);
    ui_->SetSize(kWindowWidth, kWindwoHeight);
    ui_->sort_key = 11;
    

    overlay_ = std::make_unique<SpriteComponent>(nullptr, SpriteSpace::kScreen);
    overlay_->color = kOverlayColor;
    overlay_->SetSize(kWindowWidth, kWindwoHeight);
    overlay_->sort_key = 10;

    Texture2D* tex_sele = TextureManager::Get().Load(L"Assets/Texture/select.png");
    cur_texture_ = std::make_unique<SpriteComponent>(tex_sele, SpriteSpace::kScreen);
    cur_texture_->SetSize(kCursorWidth, kCursorHeight);
    cur_texture_->SetPos(kCursorX, kCursorStartY);
    cur_texture_->sort_key = 12;

    ui_->SetVisible(visible_);
    overlay_->SetVisible(visible_);
    cur_texture_->SetVisible(visible_);
}

Option::~Option()
{
    OnDetach();
}


void Option::OnAttach(const AttachContext& context) const
{
    ui_->OnAttach(context);
    overlay_->OnAttach(context);
    cur_texture_->OnAttach(context);
}

void Option::OnDetach() const
{
    ui_->OnDetach();
    overlay_->OnDetach();
    cur_texture_->OnDetach();
}

void Option::Tick(float dt)
{
    if (game_main->GetInput().CheckKey(InputKey::kEsc, KeyState::kPressed))
    {
        SetOpen();
    }
    if (!visible_)
    {
        return;
    }
    ui_->Tick(dt);
    overlay_->Tick(dt);
    cur_texture_->Tick(dt);
    Input();
}


void Option::Input()
{
    if (game_main->GetInput().CheckKey(InputKey::kW, KeyState::kPressed))
    {
        button_index_--;
    }
    if (game_main->GetInput().CheckKey(InputKey::kS, KeyState::kPressed))
    {
        button_index_++;
    }
    if (game_main->GetInput().CheckKey(InputKey::kEnter, KeyState::kPressed))
    {
        const auto button = static_cast<OptionButton>(button_index_);
        switch (button)
        {
        case OptionButton::kRestart: SetOpen();
            break;
        case OptionButton::kExit: game_main->Quit();
            break;
        case OptionButton::kSelect : 
            game_main->GetGameInstance().GetLevelManager().OpenLevel(LevelName::kSelect);
            SetOpen();
            break;
        default: 
            break;
        }
    }
    button_index_ = std::clamp(button_index_, kMinSelectionIndex, kMaxSelectionIndex);
    cur_texture_->SetPos(kCursorX, kCursorStartY + button_index_ * kCursorStepY);
}

void Option::SetOpen()
{
    SetVisible(!visible_);
    game_main->GetGameInstance().SetUseTick(visible_);
    ui_->SetVisible(visible_);
    overlay_->SetVisible(visible_);
    cur_texture_->SetVisible(visible_);
    button_index_ = 0;
}

bool Option::IsVisible() const
{
    return visible_;
}

void Option::SetVisible(bool visible)
{
    visible_ = visible;
}

#include "title.h"

#include "../../Core/common_data.h"
#include "../../Engine/Components/sprite_component.h"
#include "../../Game/GameMain.h"
#include "../../Resource/audio_manager.h"

Title::Title()
{
    Texture2D* tex = TextureManager::Get().Load(L"Assets/Texture/title.png");
    ui_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    ui_->SetSize(kWindowWidth, kWindwoHeight);
    ui_->sort_key = 1;

    overlay_ = AddComponent<SpriteComponent>(nullptr, SpriteSpace::kScreen);
    overlay_->color = kOverlayColor;
    overlay_->SetSize(kWindowWidth, kWindwoHeight);
    overlay_->sort_key = 0;

    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/selectdown.wav", Sound::kSelectDown.c_str(), false);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/selectup.wav", Sound::kSelectUp.c_str(), false);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/click1.wav", Sound::kClick1.c_str(), false);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/click2.wav", Sound::kClick2.c_str(), false);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/option.wav", Sound::kOption.c_str(), false);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/clear.wav", Sound::kClear.c_str(), false);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/game.wav", Sound::kGame.c_str(), true);
    AudioManager::GetInstance().LoadWaveFile("Assets/Sound/8bit_Game_Menu.wav", Sound::kTitle.c_str(), true);
    AudioManager::GetInstance().Play(Sound::kTitle);
    AudioManager::GetInstance().SetVolume(Sound::kTitle,kHalfSize);
}

void Title::Tick(float dt)
{
    if (game_main->GetInput().CheckKey(InputKey::kEnter, KeyState::kDown))
    {
        AudioManager::GetInstance().Play(Sound::kClick1);
        //AudioManager::GetInstance().Play(Sound::kClick2.c_str());
        game_main->GetGameInstance().GetLevelManager().OpenLevel(LevelName::kSelect);
    }
    Actor::Tick(dt);
}

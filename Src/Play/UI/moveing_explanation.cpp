#include "moveing_explanation.h"

#include "../../Engine/Components/sprite_component.h"

void MoveingExplanation::Begin()
{
    Texture2D* tex = TextureManager::Get().Load(L"Assets/Texture/explanation.png");
    explanation_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    explanation_->SetSize(kWindowWidth, kWindwoHeight);
    explanation_->sort_key = 0;
    Actor::Begin();
}

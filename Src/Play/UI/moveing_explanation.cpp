#include "moveing_explanation.h"

#include "../../Engine/Components/sprite_component.h"

Explanation::Explanation(std::wstring_view path)
{
    path_ = path;
}

void Explanation::Begin()
{
    Texture2D* tex = TextureManager::Get().Load(path_.c_str());
    explanation_ = AddComponent<SpriteComponent>(tex, SpriteSpace::kScreen);
    explanation_->SetSize(kWindowWidth, kWindwoHeight);
    explanation_->sort_key = 0;
    Actor::Begin();
}

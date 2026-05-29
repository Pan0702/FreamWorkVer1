#include "sprite_component.h"

#include "../attach_context.h"
#include "../../Renderer/sprite_renderer.h"
#include "../../Renderer/ui_renderer.h"

SpriteComponent::SpriteComponent(Texture2D* texture, SpriteSpace space)
    : texture(texture), space(space)
{
}

SpriteComponent::~SpriteComponent()
{
    OnDetach();
}

void SpriteComponent::OnAttach(const AttachContext& context)
{
    if (space == SpriteSpace::kScreen && context.ui_renderer != nullptr)
    {
        ui_renderer_ = context.ui_renderer;
        ui_renderer_->Register(this);
        return;
    }

    sprite_renderer_ = context.sprite_renderer;
    if (sprite_renderer_ != nullptr)
    {
        sprite_renderer_->Register(this);
    }
}

void SpriteComponent::OnDetach()
{
    if (sprite_renderer_ != nullptr)
    {
        sprite_renderer_->Unregister(this);
        sprite_renderer_ = nullptr;
    }
    if (ui_renderer_ != nullptr)
    {
        ui_renderer_->Unregister(this);
        ui_renderer_ = nullptr;
    }
}

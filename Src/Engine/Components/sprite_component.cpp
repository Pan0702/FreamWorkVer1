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

void SpriteComponent::SetSize(float w, float h)
{
    size_rate_.x = w / kWindowWidth;
    size_rate_.y = h / kWindwoHeight;
    is_relative_size_ = true;
}

void SpriteComponent::SetPos(float w, float h)
{
    pos_rate_.x = w / kWindowWidth;
    pos_rate_.y = h / kWindwoHeight;
    is_relative_pos_ = true;
}

const Vec2& SpriteComponent::GetSize() const
{
    return size_rate_;
}

bool SpriteComponent::GetRelativeSizeFlag() const
{
    return is_relative_size_;
}

const Vec2& SpriteComponent::GetPos() const
{
    return pos_rate_;
}

bool SpriteComponent::GetRelativePosFlag() const
{
    return is_relative_pos_;
}

bool SpriteComponent::GetVisible() const
{
    return visible_;
}

void SpriteComponent::SetVisible(bool visible)
{
    visible_ = visible;
}

#pragma once
#include "../../Core/Math/my_math.h"
#include "../component.h"

class SpriteRenderer;
class Texture2D;
class UIRenderer;

enum class SpriteSpace
{
    kWorld,
    kScreen,
};

class SpriteComponent : public Component
{
public:
    SpriteComponent() = default;
    SpriteComponent(Texture2D* texture, SpriteSpace space);
    ~SpriteComponent() override;

    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;

    Texture2D* texture = nullptr;
    Vec2 position = {0.0f, 0.0f};
    Vec2 size = {64.0f, 64.0f};
    Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec4 src_rect = {0.0f, 0.0f, 1.0f, 1.0f};
    SpriteSpace space = SpriteSpace::kWorld;

private:
    SpriteRenderer* sprite_renderer_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
};

#include "ui_renderer.h"

bool UIRenderer::Initialize(ID3D12Device* device)
{
    return sprite_renderer_.Initialize(device);
}

void UIRenderer::Shutdown()
{
    sprite_renderer_.Shutdown();
}

void UIRenderer::Register(SpriteComponent* component)
{
    sprite_renderer_.Register(component);
}

void UIRenderer::Unregister(SpriteComponent* component)
{
    sprite_renderer_.Unregister(component);
}

void UIRenderer::DrawImmediate(const SpriteDrawCommand& command)
{
    sprite_renderer_.DrawImmediate(command);
}

void UIRenderer::Collect()
{
    sprite_renderer_.Collect();
}

void UIRenderer::Sort()
{
    sprite_renderer_.Sort();
}

void UIRenderer::Submit(RenderContext& context)
{
    sprite_renderer_.Submit(context);
}

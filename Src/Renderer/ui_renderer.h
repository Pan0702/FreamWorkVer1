#pragma once
#include <memory>

#include "sprite_renderer.h"

class SpriteComponent;

class UIRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void Shutdown();
    void Register(SpriteComponent* component);
    void Unregister(SpriteComponent* component);
    void DrawImmediate(const SpriteDrawCommand& command);
    void Collect();
    void Sort();
    void Submit(RenderContext& context);

private:
    SpriteRenderer sprite_renderer_;
};

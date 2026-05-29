#pragma once

class MeshRenderer;
class SpriteRenderer;
class UIRenderer;

struct AttachContext
{
    MeshRenderer* mesh_renderer = nullptr;
    SpriteRenderer* sprite_renderer = nullptr;
    UIRenderer* ui_renderer = nullptr;
};

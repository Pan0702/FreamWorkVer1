#pragma once

class SkinnedMeshRenderer;
class MeshRenderer;
class SpriteRenderer;
class UIRenderer;
class CollisionWorld;

/**
 * @brief AttachContextのデータと処理をまとめる型。
 */
struct AttachContext
{
    MeshRenderer* mesh_renderer = nullptr;
    SpriteRenderer* sprite_renderer = nullptr;
    UIRenderer* ui_renderer = nullptr;
    SkinnedMeshRenderer* skinned_mesh_renderer = nullptr;
    CollisionWorld* collision_world = nullptr;
};

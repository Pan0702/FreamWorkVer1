#pragma once

class NavigationSystem;
class InstancedMeshRenderer;
class SkinnedMeshRenderer;
class MeshRenderer;
class SpriteRenderer;
class UIRenderer;
class CollisionWorld;

/**
 * @brief AttachContext‚Ìƒf[ƒ^‚Æˆ—‚ğ‚Ü‚Æ‚ß‚éŒ^
 */
struct AttachContext
{
    NavigationSystem* navigation_system = nullptr;
    MeshRenderer* mesh_renderer = nullptr;
    SpriteRenderer* sprite_renderer = nullptr;
    UIRenderer* ui_renderer = nullptr;
    SkinnedMeshRenderer* skinned_mesh_renderer = nullptr;
    InstancedMeshRenderer* instanced_mesh_renderer = nullptr;
    CollisionWorld* collision_world = nullptr;
};

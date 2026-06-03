#include "game_instance.h"

#include "render_system.h"
#include "../Game/GameMain.h"
#include "../Renderer/scene_renderer.h"

bool GameInstance::Initialize()
{
    SceneRenderer* scene_renderer = game_main->GetRenderSystem()->GetSceneRenderer();
    
    AttachContext attach_context = {};
    attach_context.mesh_renderer = scene_renderer->GetMeshRenderer();
    attach_context.sprite_renderer = scene_renderer->GetSpriteRenderer();
    attach_context.ui_renderer = scene_renderer->GetUIRenderer();
    world_.SetAttachContext(attach_context);
    level_manager_.Initialize(&world_);
    level_manager_.OpenLevel("Test");
    return true;
}

void GameInstance::Tick(float dt)
{
    level_manager_.Tick(dt);
    world_.Tick(dt);
}

void GameInstance::Render()
{
    game_main->GetRenderSystem()->Render(&world_,&game_main->GetCamera());
}

World* GameInstance::GetWorld()
{
    return &world_;
}

LevelManager& GameInstance::GetLevelManager()
{
    return level_manager_;
}

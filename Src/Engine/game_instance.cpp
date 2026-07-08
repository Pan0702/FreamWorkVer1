#include "game_instance.h"

#include "render_system.h"
#include "../Game/GameMain.h"

#include "../Renderer/scene_renderer.h"
#include "../Resource/animation.h"
#include "../Play/UI/option.h"

GameInstance::GameInstance() = default;

GameInstance::~GameInstance() = default;

bool GameInstance::Initialize()
{
    SceneRenderer* scene_renderer = game_main->GetRenderSystem()->GetSceneRenderer();

    AttachContext attach_context = {};
    attach_context.mesh_renderer = scene_renderer->GetMeshRenderer();
    attach_context.sprite_renderer = scene_renderer->GetSpriteRenderer();
    attach_context.ui_renderer = scene_renderer->GetUIRenderer();
    attach_context.skinned_mesh_renderer = scene_renderer->GetSkinnedMeshRenderer();
    world_.SetAttachContext(attach_context);
    option_ = std::make_unique<Option>();
    option_->OnAttach(attach_context);
    level_manager_.Initialize(&world_);
    level_manager_.OpenLevelImmediate(LevelName::kTest);
    return true;
}

void GameInstance::Tick(float dt)
{
    option_->Tick(dt);
    level_manager_.Tick(dt);
    if (!use_tick_)return;
    world_.Tick(dt);
}

void GameInstance::Render()
{
    game_main->GetRenderSystem()->Render();
}

World* GameInstance::GetWorld()
{
    return &world_;
}

LevelManager& GameInstance::GetLevelManager()
{
    return level_manager_;
}

void GameInstance::SetUseTick(bool use_tick)
{
    use_tick_ = !use_tick;
}

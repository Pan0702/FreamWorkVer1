#include "application.h"
#include "actor.h"
#include "Components/transform_component.h"
#include "world.h"
#include "../Platform/window.h"
#include "../Core/Math/my_math.h"
#include "../Debug/debug_scene.h"
#include "../Renderer/scene_renderer.h"
#include "../Debug/debug.h"
#include "../Game/GameMain.h"
#include "../Engine/render_system.h"

bool Application::Initialize()
{
    SceneRenderer* scene_render = game_main->GetRenderSystem()->GetSceneRenderer();
    world_ = std::make_unique<World>();
    AttachContext attach_context = {};
    attach_context.mesh_renderer = scene_render->GetMeshRenderer();
    attach_context.sprite_renderer = scene_render->GetSpriteRenderer();
    attach_context.ui_renderer = scene_render->GetUIRenderer();
    world_->SetAttachContext(attach_context);

    auto actor = std::make_unique<Actor>();
    auto* transform = actor->AddComponent<TransformComponent>();
    transform->position = Vec3(0.0f, 0.0f, 0.0f);
    test_actor_ = actor.get();
    actor->AddComponent<DebugComponent>();
    world_->AddActor(std::move(actor));


    DEBUG_LOG("Application initialized");
    return true;
}

void Application::Tick(float dt)
{
    world_->Tick(dt);
}

void Application::Render()
{
    game_main->GetRenderSystem()->Render(world_.get(), &game_main->GetCamera());
}

#include "application.h"
#include "actor.h"
#include "Components/transform_component.h"
#include "world.h"
#include "../Platform/window.h"
#include "../Core/Math/my_math.h"
#include "../Resource/texture_manager.h"
#include "../Debug/debug.h"
#include "../Debug/debug_scene.h"
#include "render_system.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/depth_stencil.h"
#include "../Renderer/scene_renderer.h"
Application::Application() = default;

Application::~Application() = default;

bool Application::Initialize(const wchar_t* title, uint32_t width, uint32_t height)
{
    if (!window_.Create(title, width, height))
    {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
        return false;
    }

    input_.Initialize(window_.GetHwnd());
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera_.Initialize(kHalfPi, aspect, 0.1f, 100.0f);

    render_system_ = std::make_unique<RenderSystem>();
    if (!render_system_->Initialize(&window_))
    {
        return false;
    }

    RegisterResizeCallback();

    SceneRenderer* scene_render = render_system_->GetSceneRenderer();
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

    window_.Show();
    camera_.pos_ = Vec3(0, 0, 5.0f);
    camera_.look_ = Vec3(0, 0, -1.0f);
    DEBUG_LOG("Application initialized");
    return true;
}
void Application::Run()
{
    while (window_.ProcessMessages())
    {
        Update();
        Render();
    }
    WaitForGPU();
}

void Application::Shutdown()
{
    Debug::Get().Shutdown();
    TextureManager::Get().Shutdown();
    if (render_system_ != nullptr)
    {
        render_system_->Shutdown();
    }
}

void Application::Update()
{
    input_.Update();
    world_->Tick(0.1f);
}

void Application::Render()
{
    render_system_->Render(world_.get(), &camera_);
}

void Application::WaitForGPU() const
{
    render_system_->WaitForGPU();
}

void Application::RegisterResizeCallback()
{
    window_.SetResizeCallback([this](uint32_t w, uint32_t h)
    {
        // 最小化中は 0 サイズになるので GPU リソースを作り直さない。
        if (w == 0 || h == 0)
        {
            return;
        }

        WaitForGPU();
        render_system_->GetSwapChain()->Resize(w, h);
        render_system_->GetDepthStencil()->Resize(w, h);
        camera_.SetAspect(static_cast<float>(w) / static_cast<float>(h));
    });
}
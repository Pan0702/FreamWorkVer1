#include "application.h"
#include "actor.h"
#include "camera.h"
#include "Components/transform_component.h"
#include "world.h"
#include "../Platform/window.h"
#include "../Graphics/graphics_device.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/command_queue.h"
#include "../Graphics/command_list.h"
#include "../Graphics/depth_stencil.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Core/Math/my_math.h"
#include "../Platform/input.h"
#include "../Renderer/scene_renderer.h"
#include "../Resource/texture_manager.h"
#include "../Debug/debug.h"
#include "../Debug/debug_scene.h"
#include "../Renderer/debug_line_renderer.h"

Application::Application() = default;

Application::~Application() = default;

bool Application::Initialize(const wchar_t* title, uint32_t width, uint32_t height)
{
    window_ = std::make_unique<Window>();
    if (!window_->Create(title, width, height))
    {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
        return false;
    }

    graphics_device_ = std::make_unique<GraphicsDevice>();
#ifdef _DEBUG
    constexpr bool enable_debug = true;
#else
    constexpr bool enable_debug = false;
#endif
    if (!graphics_device_->Initialize(enable_debug))
    {
        MessageBox(nullptr, L"Failed to create graphics device", L"Error", MB_OK);
        return false;
    }

    command_queue_ = std::make_unique<CommandQueue>();
    if (!command_queue_->Initialize(graphics_device_->GetDevice()))
    {
        MessageBox(nullptr, L"Failed to create command queue", L"Error", MB_OK);
        return false;
    }

    swap_chain_ = std::make_unique<SwapChain>();
    if (!swap_chain_->Initialize(graphics_device_->GetFactory(), graphics_device_->GetDevice(),
                                 command_queue_->GetCommandQueue(), window_->GetHwnd(), width, height))
    {
        MessageBox(nullptr, L"Failed to create swap chain", L"Error", MB_OK);
        return false;
    }

    command_list_ = std::make_unique<CommandList>();
    if (!command_list_->Initialize(graphics_device_->GetDevice()))
    {
        MessageBox(nullptr, L"Failed to create command list", L"Error", MB_OK);
        return false;
    }


    depth_stencil_ = std::make_unique<DepthStencil>();
    if (!depth_stencil_->Initialize(graphics_device_->GetDevice(), window_->GetWidth(), window_->GetHeight()))
    {
        MessageBox(nullptr, L"Failed to create depth stencil", L"Error", MB_OK);
        return false;
    }
    srv_heap_ = std::make_unique<DescriptorHeap>();
    if (!srv_heap_->Initialize(graphics_device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 64, true))
    {
        MessageBox(nullptr, L"Failed to create descriptor heap", L"Error", MB_OK);
        return false;
    }
    TextureManager::Get().Initialize(graphics_device_->GetDevice(), srv_heap_.get(),
                                     command_queue_.get(), command_list_.get());
    cb_allocator_ = std::make_unique<ConstantBufferAllocator>();
    constexpr uint32_t kConstantBufferAllocatorSize = 1024 * 1024;
    if (!cb_allocator_->Initialize(graphics_device_->GetDevice(), kConstantBufferAllocatorSize))
    {
        MessageBox(nullptr, L"Failed to create constant buffer allocator", L"Error", MB_OK);
        return false;
    }
    input_ = std::make_unique<Input>();
    input_->Initialize(window_->GetHwnd());
    camera_ = std::make_unique<Camera>();
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera_->Initialize(kHalfPi, aspect, 0.1f, 100.0f);

    window_->SetResizeCallback([this](uint32_t w, uint32_t h)
    {
        WaitForGPU();
        swap_chain_->Resize(w, h);
        depth_stencil_->Resize(w, h);
        camera_->SetAspect(static_cast<float>(w) / static_cast<float>(h));
    });
    scene_renderer_ = std::make_unique<SceneRenderer>();
    if (!scene_renderer_->Initialize(graphics_device_->GetDevice(), window_->GetHwnd(),command_queue_->GetCommandQueue(),kFrameCount))
    {
        MessageBox(nullptr, L"Failed to create scene renderer", L"Error", MB_OK);
        return false;
    }
    world_ = std::make_unique<World>();
    AttachContext attach_context = {};
    attach_context.mesh_renderer = scene_renderer_->GetMeshRenderer();
    attach_context.sprite_renderer = scene_renderer_->GetSpriteRenderer();
    attach_context.ui_renderer = scene_renderer_->GetUIRenderer();
    world_->SetAttachContext(attach_context);

    auto actor = std::make_unique<Actor>();
    auto* transform = actor->AddComponent<TransformComponent>();
    transform->position = Vec3(0.0f, 0.0f, 0.0f);
    test_actor_ = actor.get();
    actor->AddComponent<DebugComponent>();
    world_->AddActor(std::move(actor));
    debug_line_renderer_ = std::make_unique<DebugLineRenderer>();
    Debug::Get().Initialize(scene_renderer_->GetSpriteRenderer(), scene_renderer_->GetUIRenderer(),debug_line_renderer_.get());
    window_->Show();
    DEBUG_LOG("Application initialized");
    return true;
}

void Application::Run()
{
    while (window_->ProcessMessages())
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
    scene_renderer_->Shutdown();
}

void Application::Update()
{
    input_->Update();
    world_->Tick(0.1f);
    camera_->pos_ = Vec3(0, 0, 5.0f);
    camera_->look_ = Vec3(0, 0, -1.0f);
}

void Application::Render()
{
    if (!command_list_->Reset())
    {
        MessageBox(nullptr, L"Failed to reset command list", L"Error", MB_OK);
        return;
    }
    cb_allocator_->Reset();
    auto command_list = command_list_->GetCommandList();
    
    RendererData renderer_data = {};
    renderer_data.command_list = command_list_.get();
    renderer_data.command_queue = command_queue_.get();
    renderer_data.depth_stencil = depth_stencil_.get();
    renderer_data.srv_heap = srv_heap_.get();
    renderer_data.cb_allocator = cb_allocator_.get();
    renderer_data.swap_chain = swap_chain_.get();
    renderer_data.window = window_.get();
    scene_renderer_->Render(renderer_data, world_.get(), camera_.get());
    if (!command_list_->Close())
    {
        MessageBox(nullptr, L"Failed to close command list", L"Error", MB_OK);
        return;
    }

    ID3D12CommandList* command_lists[] = {command_list};
    command_queue_->GetCommandQueue()->ExecuteCommandLists(1, command_lists);
    swap_chain_->Present();
    command_queue_->WaitIdle();
}

void Application::WaitForGPU() const
{
    command_queue_->WaitIdle();
}

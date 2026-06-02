#include "render_system.h"
#include "../Platform/window.h"
#include "../Graphics/graphics_device.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/command_queue.h"
#include "../Graphics/command_list.h"
#include "../Graphics/depth_stencil.h"
#include "../Graphics/constant_buffer_allocator.h"
#include "../Core/Math/my_math.h"
#include "../Resource/texture_manager.h"
#include "../Renderer/debug_line_renderer.h"
#include "../Renderer/scene_renderer.h"
#include "../Debug/debug.h"

RenderSystem::RenderSystem() = default;
RenderSystem::~RenderSystem() = default;

bool RenderSystem::Initialize(Window* window)
{
    window_ = window;

    // ウィンドウが無いと swap chain を作れない。
    if (window_ == nullptr)
    {
        MessageBox(nullptr, L"Invalid window", L"Error", MB_OK);
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
                                 command_queue_->GetCommandQueue(), window_->GetHwnd(), window_->GetWidth(),
                                 window_->GetHeight()))
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

    scene_renderer_ = std::make_unique<SceneRenderer>();
    if (!scene_renderer_->Initialize(graphics_device_->GetDevice(), window_->GetHwnd(),
                                     command_queue_->GetCommandQueue(), kFrameCount))
    {
        MessageBox(nullptr, L"Failed to create scene renderer", L"Error", MB_OK);
        return false;
    }

    Debug::Get().Initialize(scene_renderer_->GetSpriteRenderer(), scene_renderer_->GetUIRenderer(),
                            scene_renderer_->GetDebugLineRenderer());
    return true;
}
void RenderSystem::Render(World* world, Camera* camera)
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
    renderer_data.window = window_;
    scene_renderer_->Render(renderer_data, world, camera);
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

void RenderSystem::Shutdown()
{
    if (command_queue_ != nullptr)
    {
        command_queue_->WaitIdle();
    }

    if (scene_renderer_ != nullptr)
    {
        scene_renderer_->Shutdown();
    }
}

void RenderSystem::WaitForGPU() const
{
    command_queue_->WaitIdle();
}

SwapChain* RenderSystem::GetSwapChain() const
{
    return swap_chain_.get();
}

CommandList* RenderSystem::GetCommandList() const
{
    return command_list_.get();
}

DescriptorHeap* RenderSystem::GetDescriptorHeap() const
{
    return srv_heap_.get();
}

ConstantBufferAllocator* RenderSystem::GetConstantBufferAllocator() const
{
    return cb_allocator_.get();
}

SceneRenderer* RenderSystem::GetSceneRenderer() const
{
    return scene_renderer_.get();
}

DebugLineRenderer* RenderSystem::GetDebugLineRenderer() const
{
    if (scene_renderer_ == nullptr)
    {
        return nullptr;
    }
    return scene_renderer_->GetDebugLineRenderer();
}

DepthStencil* RenderSystem::GetDepthStencil() const
{
    return depth_stencil_.get();
}

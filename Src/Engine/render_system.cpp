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
#include "../Resource/mesh_manager.h"
#include "../Resource/skeltal_mesh_manager.h"
#include "../Graphics/graphics_config.h"
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

    constexpr uint32_t kCBAllocatorSize = 1024 * 1024;
    for (auto& frame : frames_)
    {
        if (!frame.Initialize(graphics_device_->GetDevice(), kCBAllocatorSize))
        {
            MessageBox(nullptr, L"Failed to create frame resource", L"Error", MB_OK);
            return false;
        }
    }

    depth_stencil_ = std::make_unique<DepthStencil>();
    if (!depth_stencil_->Initialize(graphics_device_->GetDevice(), window_->GetWidth(), window_->GetHeight()))
    {
        MessageBox(nullptr, L"Failed to create depth stencil", L"Error", MB_OK);
        return false;
    }

    constexpr uint32 init_srv_heap_size = 1024;
    srv_heap_ = std::make_unique<DescriptorHeap>();
    if (!srv_heap_->Initialize(graphics_device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                               init_srv_heap_size, true))
    {
        MessageBox(nullptr, L"Failed to create descriptor heap", L"Error", MB_OK);
        return false;
    }

    TextureManager::Get().Initialize(graphics_device_->GetDevice(), srv_heap_.get(),
                                     command_queue_.get(), command_list_.get());

    MeshManager::Get().Initialize(graphics_device_->GetDevice());
    SkeletalMeshManager::Get().Initialize(graphics_device_->GetDevice());


    scene_renderer_ = std::make_unique<SceneRenderer>();
    if (!scene_renderer_->Initialize(graphics_device_->GetDevice(), window_->GetHwnd(),
                                     command_queue_->GetCommandQueue(), kFrameCount, srv_heap_.get()))
    {
        MessageBox(nullptr, L"Failed to create scene renderer", L"Error", MB_OK);
        return false;
    }

    Debug::Get().Initialize(scene_renderer_->GetSpriteRenderer(), scene_renderer_->GetUIRenderer(),
                            scene_renderer_->GetDebugLineRenderer());
    return true;
}

void RenderSystem::Render()
{
    uint32 index = frame_count_ % kFrameCount;
    command_queue_->WaitForFence(frames_[index].fence_value);

    frames_[index].command_allocator->Reset();
    command_list_->Reset(frames_[index].command_allocator.Get());
    auto command_list = command_list_->GetCommandList();

    RendererData renderer_data = {};
    renderer_data.command_list = command_list_.get();
    renderer_data.command_queue = command_queue_.get();
    renderer_data.depth_stencil = depth_stencil_.get();
    renderer_data.srv_heap = srv_heap_.get();
    renderer_data.cb_allocator = &frames_[index].cb_allocator;
    renderer_data.swap_chain = swap_chain_.get();
    renderer_data.window = window_;
    frames_[index].cb_allocator.Reset();
    scene_renderer_->Render(renderer_data);
    if (!command_list_->Close())
    {
        MessageBox(nullptr, L"Failed to close command list", L"Error", MB_OK);
        return;
    }

    //　GPUにコマンドを送信 //
    ID3D12CommandList* command_lists[] = {command_list_.get()->GetCommandList()};
    command_queue_->GetCommandQueue()->ExecuteCommandLists(1, command_lists);
    swap_chain_->Present();
    frames_[index].fence_value = command_queue_->Signal();
    ++frame_count_;
    
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

ID3D12Device* RenderSystem::GetDevice() const
{
    return graphics_device_->GetDevice();
}

bool RenderSystem::FrameResource::Initialize(ID3D12Device* device, uint32 cb_size)
{
    HRESULT hr = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&command_allocator));

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create command allocator", L"Error", MB_OK);
        return false;
    }
    if (!cb_allocator.Initialize(device, cb_size))
    {
        MessageBox(nullptr, L"Failed to create constant buffer allocator", L"Error", MB_OK);
        return false;
    }
    return true;
}

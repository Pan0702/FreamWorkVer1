#pragma once
#include "../Core/common.h"
#include <memory>
class DebugLineRenderer;
class SceneRenderer;
class Camera;
class Input;
class Actor;
class World;
class DescriptorHeap;
class ConstantBufferAllocator;
class DepthStencil;
class Window;
class GraphicsDevice;
class CommandQueue;
class SwapChain;
class CommandList;



class RenderSystem
{
public:
    RenderSystem();
    ~RenderSystem();
    bool Initialize(Window* window);
    void Render(World* world, Camera* camera) const;
    void Shutdown();
    void WaitForGPU() const;

    SwapChain* GetSwapChain() const;
    CommandList* GetCommandList() const;
    DescriptorHeap* GetDescriptorHeap() const;
    ConstantBufferAllocator* GetConstantBufferAllocator() const;
    SceneRenderer* GetSceneRenderer() const;
    DebugLineRenderer* GetDebugLineRenderer() const;
    DepthStencil* GetDepthStencil() const;
    ID3D12Device* GetDevice() const;
    float GetLastPresentMs() const { return last_present_ms_; }
    float GetLastGpuWaitMs() const { return last_gpu_wait_ms_; }
    
private:
    std::unique_ptr<GraphicsDevice> graphics_device_;
    std::unique_ptr<CommandQueue> command_queue_;
    std::unique_ptr<SwapChain> swap_chain_;
    std::unique_ptr<CommandList> command_list_;
    std::unique_ptr<DepthStencil> depth_stencil_;
    std::unique_ptr<DescriptorHeap> srv_heap_;
    std::unique_ptr<ConstantBufferAllocator> cb_allocator_;
    std::unique_ptr<SceneRenderer> scene_renderer_;
    
    Window* window_ = nullptr;
    mutable float last_present_ms_ = 0.0f;
    mutable float last_gpu_wait_ms_ = 0.0f;
};

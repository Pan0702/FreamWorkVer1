#pragma once
#include "../Core/common.h"
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


class Application
{
public:
    Application();
    ~Application();

    bool Initialize(const wchar_t* title, uint32_t width, uint32_t height);
    void Run();
    void Shutdown();

private:
    void Update();
    void Render();
    void WaitForGPU() const;

    std::unique_ptr<Window> window_;
    std::unique_ptr<GraphicsDevice> graphics_device_;
    std::unique_ptr<CommandQueue> command_queue_;
    std::unique_ptr<SwapChain> swap_chain_;
    std::unique_ptr<CommandList> command_list_;
    std::unique_ptr<DepthStencil> depth_stencil_;
    std::unique_ptr<DescriptorHeap> srv_heap_;
    std::unique_ptr<ConstantBufferAllocator> cb_allocator_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<SceneRenderer> scene_renderer_;
    std::unique_ptr<World> world_;
    Actor* test_actor_ = nullptr;
    std::unique_ptr<DebugLineRenderer> debug_line_renderer_;
};

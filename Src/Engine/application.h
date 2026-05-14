#pragma once
#include "../Core/common.h"

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
};

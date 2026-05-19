#pragma once
#include "../Core/common.h"

class VertexBuffer;
class PipelineState;
class Shader;
class RootSignature;
class Window;
class GraphicsDevice;
class CommandQueue;
class SwapChain;
class CommandList;
class IndexBuffer;
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
    std::unique_ptr<RootSignature>  root_signature_;
    std::unique_ptr<Shader>         vertex_shader_;
    std::unique_ptr<Shader>         pixel_shader_;
    std::unique_ptr<PipelineState>  pipeline_state_;
    std::unique_ptr<VertexBuffer>   vertex_buffer_;
    std::unique_ptr<IndexBuffer>   index_buffer_;
};

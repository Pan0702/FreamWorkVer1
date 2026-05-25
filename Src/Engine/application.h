#pragma once
#include "../Core/common.h"

class Camera;
class Input;
class DescriptorHeap;
class DepthStencil;
class ConstantBuffer;
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
class Texture2D;


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
    std::unique_ptr<ConstantBuffer> constant_buffer_;
    std::unique_ptr<DepthStencil> depth_stencil_;
    std::unique_ptr<DescriptorHeap> srv_heap_;
    std::unique_ptr<Texture2D> texture_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<Camera> camera_;
    float rotation_ = 0.0f;
};

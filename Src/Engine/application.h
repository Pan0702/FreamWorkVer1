#pragma once
#include "../Core/common.h"
class Material;
class SceneRenderer;
class RenderObject;
class Scene;
class Camera;
class Input;
class DescriptorHeap;
class DepthStencil;
class Window;
class GraphicsDevice;
class CommandQueue;
class SwapChain;
class CommandList;
class Mesh;
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
    std::unique_ptr<DepthStencil> depth_stencil_;
    std::unique_ptr<DescriptorHeap> srv_heap_;
    std::unique_ptr<Texture2D> texture_;
    std::unique_ptr<Mesh> mesh_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<SceneRenderer> scene_renderer_;
    std::unique_ptr<Scene> scene_;
    std::unique_ptr<RenderObject> render_object_;
    std::unique_ptr<Material> material_;
    float rotation_ = 0.0f;
};

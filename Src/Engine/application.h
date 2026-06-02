#pragma once
#include "../Core/common.h"
#include "../Platform/window.h"
#include "../Engine/camera.h"
#include "../Platform/input.h"

class RenderSystem;
class Actor;
class World;



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
    void RegisterResizeCallback();

    Window window_;
    Input input_;
    Camera camera_;
    std::unique_ptr<RenderSystem> render_system_;
    std::unique_ptr<World> world_;
    Actor* test_actor_ = nullptr;

};

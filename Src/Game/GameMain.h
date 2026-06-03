#pragma once
#include "../Platform/window.h"
#include "../Engine/camera.h"
#include "../Engine/game_instance.h"
#include "../Platform/input.h"
class RenderSystem;


class GameMain
{
public:
    GameMain();
    ~GameMain();
    bool Initialize(const wchar_t* title, int32_t width, int32_t height);
    void Run();
    void Shutdown();
    void Tick();
    void Render();
    
    Window& GetWindow() { return window_; }
    Input& GetInput() { return input_; }
    Camera& GetCamera() { return camera_; }
    GameInstance& GetApplication() { return game_instance_; }
    RenderSystem* GetRenderSystem() const { return render_system_.get(); }
    float GetDeltaTime() const { return delta_time_; }
private:
    void ClacFPS();
    
    Window window_;
    Input input_;
    Camera camera_;
    std::unique_ptr<RenderSystem> render_system_;
    GameInstance game_instance_;
    float delta_time_ = 0.0f;
};

extern GameMain* game_main;
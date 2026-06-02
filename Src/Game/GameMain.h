#pragma once
#include "../Engine/application.h"
#include "../Platform/window.h"
#include "../Engine/camera.h"
#include "../Platform/input.h"
#include "../Engine/level_manager.h"
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
    LevelManager& GetLevelManager() { return level_manager_; }
    Application& GetApplication() { return application_; }
    RenderSystem* GetRenderSystem() const { return render_system_.get(); }
    float GetDeltaTime() const { return delta_time_; }
private:
    void ClacFPS();
    
    Window window_;
    Input input_;
    Camera camera_;
    std::unique_ptr<RenderSystem> render_system_;
    LevelManager level_manager_;
    Application application_;
    float delta_time_ = 0.0f;
};

extern GameMain* game_main;
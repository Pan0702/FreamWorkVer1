#include "GameMain.h"
#include "../Engine/render_system.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/depth_stencil.h"
#include "../Resource/texture_manager.h"
#include "../Debug/debug.h"
#include "../Resource/mesh_manager.h"
#include "../Resource/skeltal_mesh_manager.h"
#include "../Resource/animator_manager.h"
GameMain* game_main = nullptr;
namespace
{
    LARGE_INTEGER freq;
    LARGE_INTEGER current;
    constexpr int REC_SIZE = 60;
    float record[REC_SIZE];
    int recCount = 0;
    void InitializeTime()
    {
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&current);
    }
    
    float ClacDeltaTime()
    {
        LARGE_INTEGER last = current;
        QueryPerformanceCounter(&current);
        float t = static_cast<float>(current.QuadPart - last.QuadPart) / freq.QuadPart;
        float t2 = t;
        
        // deltaTimeは、平均フレームレートの3倍を超えないように制限する（スパイク対策）
        if (recCount >= REC_SIZE)
        {
            float sum = 0;
            for (float i : record)
            {
                sum += i;
            }
            sum /= REC_SIZE;
            t2 = (std::min)(t2, sum * 3.0f);
        }
        record[recCount % REC_SIZE] = t;
        recCount++;
         return t2;
    }
}


GameMain::GameMain() = default;
GameMain::~GameMain() = default;

bool GameMain::Initialize(const wchar_t* title, int32_t width, int32_t height)
{
    game_main = this;
    if (!window_.Create(title, width, height))
    {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
    }

    input_.Initialize(window_.GetHwnd());
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera_.Initialize(kHalfPi, aspect, 0.1f, 100.0f);

    render_system_ = std::make_unique<RenderSystem>();
    if (!render_system_->Initialize(&window_))
    {
        MessageBox(nullptr, L"Failed to initialize render system", L"Error", MB_OK);
        return false;
    }

    window_.SetResizeCallback([this](uint32_t w, uint32_t h)
    {
        if (w == 0 || h == 0)
        {
            return false;
        }
        render_system_->WaitForGPU();
        render_system_->GetSwapChain()->Resize(w, h);
        render_system_->GetDepthStencil()->Resize(w, h);
        camera_.SetAspect(static_cast<float>(w) / static_cast<float>(h));
    });
    game_instance_.Initialize();
    window_.Show();
    camera_.pos_ = Vec3(0, 0, 5.0f);
    camera_.look_ = Vec3(0, 0, -1.0f);
    
    InitializeTime();
    return true;
}

void GameMain::Run()
{
    while (window_.ProcessMessages())
    {
        ClacFPS();
        Tick();
        Render();
    }
    render_system_->WaitForGPU();
}

void GameMain::Shutdown()
{
    Debug::Get().Shutdown();
    TextureManager::Get().Shutdown();
    MeshManager::Get().Shutdown();
    SkeletalMeshManager::Get().Shutdown();
    AnimatorManager::Get().Shutdown();
    if (render_system_ != nullptr)
    {
        render_system_->Shutdown();
    }
}

void GameMain::Tick()
{
    
    delta_time_ = ClacDeltaTime();
    input_.Update();
    game_instance_.Tick( delta_time_);
}

void GameMain::Render()
{
    game_instance_.Render();
}

void GameMain::ClacFPS()
{
    static float fps_timer = 0.0f;
    static int frame = 0;

    fps_timer += delta_time_;
    frame++;

    if (fps_timer >= 1.0f)
    {
        float fps = frame / fps_timer;

        wchar_t title[128];
        swprintf_s(title, L"FrameWork    FPS=%.1f", fps);
        window_.DispFPS(title);

        fps_timer = 0.0f;
        frame = 0;
    }
}


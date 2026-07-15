#include "GameMain.h"
#include "../Engine/render_system.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/depth_stencil.h"
#include "../Resource/texture_manager.h"
#include "../Debug/debug.h"
#include "../Renderer/scene_renderer.h"
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
    float last_input_update_ms = 0.0f;
    float last_game_tick_ms = 0.0f;

    double ElapsedMs(const LARGE_INTEGER& begin, const LARGE_INTEGER& end)
    {
        return static_cast<double>(end.QuadPart - begin.QuadPart) * 1000.0 /
            static_cast<double>(freq.QuadPart);
    }

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

        // deltaTime はスパイク対策として大きくなりすぎないように抑える。
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

#ifdef _DEBUG
    if (!performance_profiler_.InitializeGpu(
        render_system_->GetDevice(), render_system_->GetD3D12CommandQueue(), kFrameCount))
    {
        DEBUG_LOG("GPU performance timing is unavailable");
    }
#endif

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
        return true;
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
    //Thead起動
    render_thread_ = std::thread([this]
    {
        RenderThread();
    });
    while (true)
    {
        if (!window_.ProcessMessages())
        {
            break;
        }

#ifdef _DEBUG
        performance_profiler_.BeginFrame();
        ClacFPS();
#endif
        Tick();

        // Renderが前フレーム消費し終えるまで待つ
        render_done_.acquire();
        render_system_->GetSceneRenderer()->SwapFrame();
        // Renderに投げる → 並列で次ループへ//
        frame_ready_.release();
#ifdef _DEBUG
        performance_profiler_.EndFrame();
#endif
    }
    running_ = false;
    frame_ready_.release();
    if (render_thread_.joinable())
    {
        render_thread_.join();
    }
    render_system_->WaitForGPU();
}
void GameMain::RenderThread()
{
    while (true)
    {
        frame_ready_.acquire();
        if (!running_)
        {
            break;
        }
#ifdef _DEBUG
        render_system_->Render(&performance_profiler_);
#else
        render_system_->Render(nullptr);
#endif
        render_done_.release();
    }
}

void GameMain::Shutdown()
{
#ifdef _DEBUG
    if (render_system_ != nullptr)
    {
        render_system_->WaitForGPU();
    }
    performance_profiler_.ShutdownGpu();
#endif

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
    auto* render_system = render_system_->GetSceneRenderer();
    delta_time_ = ClacDeltaTime();
    render_system->GetImGuiManager().BeginFrame();

#ifdef _DEBUG
    performance_profiler_.BeginCpuUpdate();
#endif

    input_.Update();
    game_instance_.Tick(delta_time_);

#ifdef _DEBUG
    performance_profiler_.EndCpuUpdate();
    performance_window_.Draw(performance_profiler_.GetSnapshot());
#endif

    render_system->AllCollect(camera_);
}


void GameMain::ClacFPS() const
{
    static float fps_timer = 0.0f;
    static int frame = 0;

    fps_timer += delta_time_;
    frame++;

    if (fps_timer >= 1.0f)
    {
        float fps = static_cast<float>(frame) / fps_timer;

        wchar_t title[128];
        swprintf_s(title, L"FrameWork    FPS=%.1f", fps);
        window_.DispFPS(title);

        fps_timer = 0.0f;
        frame = 0;
    }
}


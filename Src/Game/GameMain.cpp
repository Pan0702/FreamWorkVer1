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
        
        // deltaTime�́A���σt���[�����[�g��3�{�𒴂��Ȃ��悤�ɐ�������i�X�p�C�N�΍�j
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
    double profile_timer = 0.0;
    double frame_ms_sum = 0.0;
    double message_ms_sum = 0.0;
    double tick_ms_sum = 0.0;
    double input_ms_sum = 0.0;
    double game_tick_ms_sum = 0.0;
    double render_ms_sum = 0.0;
    double present_ms_sum = 0.0;
    double gpu_wait_ms_sum = 0.0;
    uint32_t frame_count = 0;
    uint32_t message_count = 0;
    uint32_t key_message_count = 0;

    while (true)
    {
        LARGE_INTEGER frame_begin = {};
        LARGE_INTEGER after_messages = {};
        LARGE_INTEGER after_tick = {};
        LARGE_INTEGER after_render = {};
        QueryPerformanceCounter(&frame_begin);

        if (!window_.ProcessMessages())
        {
            break;
        }
        QueryPerformanceCounter(&after_messages);

        ClacFPS();
        Tick();
        QueryPerformanceCounter(&after_tick);
        Render();
        QueryPerformanceCounter(&after_render);

        const double frame_ms = ElapsedMs(frame_begin, after_render);
        profile_timer += frame_ms / 1000.0;
        frame_ms_sum += frame_ms;
        message_ms_sum += ElapsedMs(frame_begin, after_messages);
        tick_ms_sum += ElapsedMs(after_messages, after_tick);
        input_ms_sum += last_input_update_ms;
        game_tick_ms_sum += last_game_tick_ms;
        render_ms_sum += ElapsedMs(after_tick, after_render);
        present_ms_sum += render_system_->GetLastPresentMs();
        gpu_wait_ms_sum += render_system_->GetLastGpuWaitMs();
        message_count += window_.GetLastMessageCount();
        key_message_count += window_.GetLastKeyMessageCount();
        ++frame_count;

        if (profile_timer >= 1.0 && frame_count > 0)
        {
            profile_timer = 0.0;
            frame_ms_sum = 0.0;
            message_ms_sum = 0.0;
            tick_ms_sum = 0.0;
            input_ms_sum = 0.0;
            game_tick_ms_sum = 0.0;
            render_ms_sum = 0.0;
            present_ms_sum = 0.0;
            gpu_wait_ms_sum = 0.0;
            frame_count = 0;
            message_count = 0;
            key_message_count = 0;
        }
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
    render_system_->GetSceneRenderer()->GetImGuiManager().BeginFrame();
    LARGE_INTEGER input_begin = {};
    LARGE_INTEGER input_end = {};
    LARGE_INTEGER tick_end = {};
    QueryPerformanceCounter(&input_begin);
    input_.Update();
    QueryPerformanceCounter(&input_end);
    game_instance_.Tick( delta_time_);
    QueryPerformanceCounter(&tick_end);
    last_input_update_ms = static_cast<float>(ElapsedMs(input_begin, input_end));
    last_game_tick_ms = static_cast<float>(ElapsedMs(input_end, tick_end));
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


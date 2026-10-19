#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>

// CPU 計測器の初期状態。false にすると計測処理をすべてスキップする。
// 実行中も Performance ウィンドウのチェックボックスで On/Off できる。
inline constexpr bool kCpuProfilerDefaultEnabled = true;

// 計測区間。メインスレッドと描画スレッドの両方から記録される。
enum class CpuSection : uint8_t
{
    kGameTick,         // Input + GameInstance::Tick (メインスレッド)
    kCollectTotal,     // SceneRenderer::AllCollect 全体 (メインスレッド)
    kCollectMesh,      // MeshRenderer::Collect
    kCollectInstanced, // InstancedMeshRenderer::Collect
    kShadowPass,       // ShadowRenderer::RenderShadowPass (描画スレッド)
    kMainPass,         // draw_items の Submit ループ (描画スレッド)
    kRenderTotal,      // SceneRenderer::Render 全体 (描画スレッド)
    kCount
};

// フレームごとに上書きする件数。
enum class CpuCounter : uint8_t
{
    kMeshCommands,
    kInstancedCommands,
    kInstances,
    kCount
};

class CpuProfiler
{
public:
    static CpuProfiler& Get();

    bool IsEnabled() const;
    void SetEnabled(bool enabled);

    void AddTime(CpuSection section, int64_t elapsed_ns);
    void SetCounter(CpuCounter counter, uint32_t value);

    // メインスレッドで 1 フレームに 1 回呼ぶ。一定間隔ごとに平均値を確定する。
    void EndFrame();
    void DrawWindow();

private:
    using Clock = std::chrono::steady_clock;
    static constexpr size_t kSectionCount = static_cast<size_t>(CpuSection::kCount);
    static constexpr size_t kCounterCount = static_cast<size_t>(CpuCounter::kCount);
    static constexpr float kAverageIntervalSec = 0.5f;

    CpuProfiler();
    void ResetAccumulation();

    std::atomic<bool> enabled_{kCpuProfilerDefaultEnabled};
    std::array<std::atomic<int64_t>, kSectionCount> accum_ns_{};
    std::array<std::atomic<uint32_t>, kCounterCount> counters_{};

    Clock::time_point window_start_{};
    uint32_t window_frames_ = 0;

    std::array<float, kSectionCount> average_ms_{};
    std::array<float, kSectionCount> baseline_ms_{};
    std::array<uint32_t, kCounterCount> baseline_counters_{};
    bool has_baseline_ = false;
};

class CpuProfileScope
{
public:
    explicit CpuProfileScope(CpuSection section)
        : section_(section), active_(CpuProfiler::Get().IsEnabled())
    {
        if (active_)
        {
            start_ = std::chrono::steady_clock::now();
        }
    }

    ~CpuProfileScope()
    {
        if (active_)
        {
            const auto elapsed = std::chrono::steady_clock::now() - start_;
            CpuProfiler::Get().AddTime(
                section_, std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count());
        }
    }

    CpuProfileScope(const CpuProfileScope&) = delete;
    CpuProfileScope& operator=(const CpuProfileScope&) = delete;

private:
    CpuSection section_;
    bool active_;
    std::chrono::steady_clock::time_point start_{};
};

#define CPU_PROFILE_CONCAT_INNER(a, b) a##b
#define CPU_PROFILE_CONCAT(a, b) CPU_PROFILE_CONCAT_INNER(a, b)
#define CPU_PROFILE_SCOPE(section) \
    CpuProfileScope CPU_PROFILE_CONCAT(cpu_profile_scope_, __LINE__)(section)

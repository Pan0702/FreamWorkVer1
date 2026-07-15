#pragma once
#include <atomic>
#include <array>
#include <chrono>
#include <cstdint>
#include <vector>

#include "../Core/common.h"

//計測器はAIに書かせました。//
struct PerformanceSnapshot
{
    float fps = 0.0f;
    float frame_time_ms = 0.0f;
    float cpu_update_ms = 0.0f;
    float cpu_render_ms = 0.0f;
    float gpu_frame_ms = 0.0f;
    uint32_t draw_call_count = 0;
    bool gpu_timing_available = false;
};

class PerformanceProfiler
{
public:
    ~PerformanceProfiler();

    bool InitializeGpu(ID3D12Device* device, ID3D12CommandQueue* command_queue, uint32_t frame_count);
    void ShutdownGpu();

    void BeginFrame();
    void EndFrame();

    void BeginCpuUpdate();
    void EndCpuUpdate();
    void BeginCpuRender();
    void EndCpuRender();

    void BeginGpuFrame(ID3D12GraphicsCommandList* command_list, uint32_t frame_index);
    void EndGpuFrame(ID3D12GraphicsCommandList* command_list, uint32_t frame_index);
    void ReadGpuFrame(uint32_t frame_index);

    const PerformanceSnapshot& GetSnapshot() const;

private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point start_frame_{};
    Clock::time_point start_cpu_update_{};
    Clock::time_point start_cpu_render_{};
    PerformanceSnapshot snapshot_{};
    bool measuring_frame_ = false;
    bool measuring_cpu_update_ = false;
    bool measuring_cpu_render_ = false;

    std::atomic<float> latest_cpu_render_ms_ = 0.0f;
    std::atomic<float> latest_gpu_frame_ms_ = 0.0f;

    ComPtr<ID3D12QueryHeap> gpu_query_heap_;
    ComPtr<ID3D12Resource> gpu_readback_buffer_;
    uint64_t* gpu_timestamps_ = nullptr;
    uint64_t gpu_timestamp_frequency_ = 0;
    uint32_t gpu_frame_count_ = 0;
    std::vector<uint8_t> gpu_frame_valid_;
};

class PerformanceWindow
{
public:
    void Draw(const PerformanceSnapshot& snapshot);

private:
    static constexpr int kHistorySize = 120;
    std::array<float, kHistorySize> history_{};
    int history_write_index_ = 0;
    int history_count_ = 0;
};

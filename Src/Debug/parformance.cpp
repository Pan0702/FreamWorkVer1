#include "parformance.h"

#include <algorithm>

#include "ImGui/imgui.h"

PerformanceProfiler::~PerformanceProfiler()
{
    ShutdownGpu();
}

bool PerformanceProfiler::InitializeGpu(ID3D12Device* device, ID3D12CommandQueue* command_queue,
                                        uint32_t frame_count)
{
    ShutdownGpu();

    if (device == nullptr || command_queue == nullptr || frame_count == 0)
    {
        return false;
    }

    if (FAILED(command_queue->GetTimestampFrequency(&gpu_timestamp_frequency_)) ||
        gpu_timestamp_frequency_ == 0)
    {
        return false;
    }

    D3D12_QUERY_HEAP_DESC query_heap_desc = {};
    query_heap_desc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    query_heap_desc.Count = frame_count * 2;

    if (FAILED(device->CreateQueryHeap(&query_heap_desc, IID_PPV_ARGS(&gpu_query_heap_))))
    {
        ShutdownGpu();
        return false;
    }

    D3D12_HEAP_PROPERTIES heap_properties = {};
    heap_properties.Type = D3D12_HEAP_TYPE_READBACK;
    heap_properties.CreationNodeMask = 1;
    heap_properties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC buffer_desc = {};
    buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    buffer_desc.Width = sizeof(uint64_t) * query_heap_desc.Count;
    buffer_desc.Height = 1;
    buffer_desc.DepthOrArraySize = 1;
    buffer_desc.MipLevels = 1;
    buffer_desc.SampleDesc.Count = 1;
    buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (FAILED(device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&gpu_readback_buffer_))))
    {
        ShutdownGpu();
        return false;
    }

    D3D12_RANGE read_range = {0, static_cast<SIZE_T>(buffer_desc.Width)};
    if (FAILED(gpu_readback_buffer_->Map(
        0, &read_range, reinterpret_cast<void**>(&gpu_timestamps_))))
    {
        ShutdownGpu();
        return false;
    }

    gpu_frame_count_ = frame_count;
    gpu_frame_valid_.assign(frame_count, 0);
    snapshot_.gpu_timing_available = true;
    return true;
}

void PerformanceProfiler::ShutdownGpu()
{
    if (gpu_readback_buffer_ != nullptr && gpu_timestamps_ != nullptr)
    {
        D3D12_RANGE written_range = {0, 0};
        gpu_readback_buffer_->Unmap(0, &written_range);
    }

    gpu_timestamps_ = nullptr;
    gpu_readback_buffer_.Reset();
    gpu_query_heap_.Reset();
    gpu_timestamp_frequency_ = 0;
    gpu_frame_count_ = 0;
    gpu_frame_valid_.clear();
    latest_gpu_frame_ms_.store(0.0f, std::memory_order_relaxed);
    snapshot_.gpu_frame_ms = 0.0f;
    snapshot_.gpu_timing_available = false;
}

void PerformanceProfiler::BeginFrame()
{
    start_frame_ = Clock::now();
    measuring_frame_ = true;
}

void PerformanceProfiler::EndFrame()
{
    if (!measuring_frame_)
    {
        return;
    }
    const Clock::time_point frame_end = Clock::now();
    const std::chrono::duration<float, std::milli> elapsed = frame_end - start_frame_;

    snapshot_.frame_time_ms = elapsed.count();

    if (snapshot_.frame_time_ms > 0.0f)
    {
        snapshot_.fps = 1000.0f / snapshot_.frame_time_ms;
    }

    snapshot_.cpu_render_ms = latest_cpu_render_ms_.load(std::memory_order_relaxed);
    snapshot_.gpu_frame_ms = latest_gpu_frame_ms_.load(std::memory_order_relaxed);

    measuring_frame_ = false;
}

void PerformanceProfiler::BeginCpuUpdate()
{
    start_cpu_update_ = Clock::now();
    measuring_cpu_update_ = true;
}

void PerformanceProfiler::EndCpuUpdate()
{
    if (!measuring_cpu_update_)
    {
        return;
    }

    const Clock::time_point end = Clock::now();
    snapshot_.cpu_update_ms =
        std::chrono::duration<float, std::milli>(end - start_cpu_update_).count();
    measuring_cpu_update_ = false;
}

void PerformanceProfiler::BeginCpuRender()
{
    start_cpu_render_ = Clock::now();
    measuring_cpu_render_ = true;
}

void PerformanceProfiler::EndCpuRender()
{
    if (!measuring_cpu_render_)
    {
        return;
    }

    const Clock::time_point end = Clock::now();
    const float elapsed_ms =
        std::chrono::duration<float, std::milli>(end - start_cpu_render_).count();
    latest_cpu_render_ms_.store(elapsed_ms, std::memory_order_relaxed);
    measuring_cpu_render_ = false;
}

void PerformanceProfiler::BeginGpuFrame(ID3D12GraphicsCommandList* command_list, uint32_t frame_index)
{
    if (command_list == nullptr || gpu_query_heap_ == nullptr || frame_index >= gpu_frame_count_)
    {
        return;
    }

    const uint32_t query_index = frame_index * 2;
    command_list->EndQuery(gpu_query_heap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP, query_index);
}

void PerformanceProfiler::EndGpuFrame(ID3D12GraphicsCommandList* command_list, uint32_t frame_index)
{
    if (command_list == nullptr || gpu_query_heap_ == nullptr || gpu_readback_buffer_ == nullptr ||
        frame_index >= gpu_frame_count_)
    {
        return;
    }

    const uint32_t query_index = frame_index * 2;
    command_list->EndQuery(gpu_query_heap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP, query_index + 1);
    command_list->ResolveQueryData(
        gpu_query_heap_.Get(),
        D3D12_QUERY_TYPE_TIMESTAMP,
        query_index,
        2,
        gpu_readback_buffer_.Get(),
        static_cast<uint64_t>(query_index) * sizeof(uint64_t));
    gpu_frame_valid_[frame_index] = 1;
}

void PerformanceProfiler::ReadGpuFrame(uint32_t frame_index)
{
    if (gpu_timestamps_ == nullptr || gpu_timestamp_frequency_ == 0 ||
        frame_index >= gpu_frame_count_ || gpu_frame_valid_[frame_index] == 0)
    {
        return;
    }

    const uint32_t query_index = frame_index * 2;
    const uint64_t begin_tick = gpu_timestamps_[query_index];
    const uint64_t end_tick = gpu_timestamps_[query_index + 1];
    if (end_tick < begin_tick)
    {
        return;
    }

    const float elapsed_ms = static_cast<float>(
        static_cast<double>(end_tick - begin_tick) * 1000.0 /
        static_cast<double>(gpu_timestamp_frequency_));
    latest_gpu_frame_ms_.store(elapsed_ms, std::memory_order_relaxed);
}

const PerformanceSnapshot& PerformanceProfiler::GetSnapshot() const
{
    return snapshot_;
}

void PerformanceWindow::Draw(const PerformanceSnapshot& snapshot)
{
    history_[history_write_index_] =
       snapshot.frame_time_ms;

    history_write_index_ =
        (history_write_index_ + 1) % kHistorySize;

    history_count_ =
        (std::min)(history_count_ + 1, kHistorySize);

    if (ImGui::Begin("Performance"))
    {
        ImGui::Text("FPS: %.1f", snapshot.fps);
        ImGui::Text("Frame: %.3f ms", snapshot.frame_time_ms);

        ImGui::Separator();

        ImGui::Text("CPU Update: %.3f ms",
                    snapshot.cpu_update_ms);
        ImGui::Text("CPU Render: %.3f ms",
                    snapshot.cpu_render_ms);
        if (snapshot.gpu_timing_available)
        {
            ImGui::Text("GPU: %.3f ms", snapshot.gpu_frame_ms);
        }
        else
        {
            ImGui::TextUnformatted("GPU: N/A");
        }
        ImGui::Text("Draw Calls: %u",
                    snapshot.draw_call_count);

        const int values_offset =
            history_count_ == kHistorySize
                ? history_write_index_
                : 0;

        ImGui::PlotLines(
            "Frame Time",
            history_.data(),
            history_count_,
            values_offset,
            nullptr,
            0.0f,
            33.3f,
            ImVec2(0.0f, 80.0f));
    }

    ImGui::End();
}

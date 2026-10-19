#include "cpu_profiler.h"

#include <iterator>

#include <psapi.h>

#include "ImGui/imgui.h"

namespace
{
    constexpr const char* kSectionNames[] = {
        "Game Tick",
        "Collect Total",
        "  Collect Mesh",
        "  Collect Instanced",
        "Shadow Pass",
        "Main Pass",
        "Render Total",
    };
    static_assert(std::size(kSectionNames) == static_cast<size_t>(CpuSection::kCount));

    constexpr const char* kCounterNames[] = {
        "Mesh Commands",
        "Instanced Commands",
        "Instances",
    };
    static_assert(std::size(kCounterNames) == static_cast<size_t>(CpuCounter::kCount));

    constexpr double kBytesPerMb = 1024.0 * 1024.0;

    double ToMb(uint64_t bytes)
    {
        return static_cast<double>(bytes) / kBytesPerMb;
    }

    void MemoryRow(const char* name, uint64_t current, uint64_t baseline, bool has_baseline)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(name);
        ImGui::TableNextColumn();
        ImGui::Text("%.1f", ToMb(current));
        if (has_baseline)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", ToMb(baseline));
            ImGui::TableNextColumn();
            const double diff = ToMb(current) - ToMb(baseline);
            const ImVec4 color = diff <= 0.0 ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
                                             : ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            ImGui::TextColored(color, "%+.1f", diff);
        }
    }
}

CpuProfiler& CpuProfiler::Get()
{
    static CpuProfiler instance;
    return instance;
}

CpuProfiler::CpuProfiler()
{
    window_start_ = Clock::now();
}

bool CpuProfiler::InitializeGpuMemory(ID3D12Device* device)
{
    ShutdownGpuMemory();
    if (device == nullptr)
    {
        return false;
    }

    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
    {
        return false;
    }

    // device を作ったアダプタを LUID で引き当てる。
    if (FAILED(factory->EnumAdapterByLuid(device->GetAdapterLuid(), IID_PPV_ARGS(&adapter_))))
    {
        adapter_.Reset();
        return false;
    }
    return true;
}

void CpuProfiler::ShutdownGpuMemory()
{
    adapter_.Reset();
    memory_.vram_usage = 0;
    memory_.vram_budget = 0;
    memory_.shared_usage = 0;
    memory_.shared_budget = 0;
}

void CpuProfiler::UpdateMemoryStats()
{
    PROCESS_MEMORY_COUNTERS_EX counters = {};
    counters.cb = sizeof(counters);
    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), sizeof(counters)))
    {
        memory_.working_set = counters.WorkingSetSize;
        memory_.peak_working_set = counters.PeakWorkingSetSize;
        memory_.private_bytes = counters.PrivateUsage;
    }

    if (adapter_ == nullptr)
    {
        return;
    }

    DXGI_QUERY_VIDEO_MEMORY_INFO local = {};
    if (SUCCEEDED(adapter_->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &local)))
    {
        memory_.vram_usage = local.CurrentUsage;
        memory_.vram_budget = local.Budget;
    }

    DXGI_QUERY_VIDEO_MEMORY_INFO non_local = {};
    if (SUCCEEDED(adapter_->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &non_local)))
    {
        memory_.shared_usage = non_local.CurrentUsage;
        memory_.shared_budget = non_local.Budget;
    }
}

bool CpuProfiler::IsEnabled() const
{
    return enabled_.load(std::memory_order_relaxed);
}

void CpuProfiler::SetEnabled(bool enabled)
{
    enabled_.store(enabled, std::memory_order_relaxed);
    ResetAccumulation();
    average_ms_.fill(0.0f);
}

void CpuProfiler::AddTime(CpuSection section, int64_t elapsed_ns)
{
    accum_ns_[static_cast<size_t>(section)].fetch_add(elapsed_ns, std::memory_order_relaxed);
}

void CpuProfiler::SetCounter(CpuCounter counter, uint32_t value)
{
    if (!IsEnabled())
    {
        return;
    }
    counters_[static_cast<size_t>(counter)].store(value, std::memory_order_relaxed);
}

void CpuProfiler::EndFrame()
{
    if (!IsEnabled())
    {
        return;
    }

    ++window_frames_;
    const Clock::time_point now = Clock::now();
    const float elapsed_sec = std::chrono::duration<float>(now - window_start_).count();
    if (elapsed_sec < kAverageIntervalSec)
    {
        return;
    }

    // 描画スレッドはメインより 1 フレーム遅れて走るが、区間平均なので誤差は無視できる。
    for (size_t i = 0; i < kSectionCount; ++i)
    {
        const int64_t total_ns = accum_ns_[i].exchange(0, std::memory_order_relaxed);
        average_ms_[i] = static_cast<float>(
            static_cast<double>(total_ns) / 1'000'000.0 / static_cast<double>(window_frames_));
    }
    window_frames_ = 0;
    window_start_ = now;

    UpdateMemoryStats();
}

void CpuProfiler::ResetAccumulation()
{
    for (auto& accum : accum_ns_)
    {
        accum.store(0, std::memory_order_relaxed);
    }
    window_frames_ = 0;
    window_start_ = Clock::now();
}

void CpuProfiler::DrawWindow()
{
    if (!ImGui::Begin("CPU Profiler"))
    {
        ImGui::End();
        return;
    }

    bool enabled = IsEnabled();
    if (ImGui::Checkbox("Enable", &enabled))
    {
        SetEnabled(enabled);
    }

    if (!enabled)
    {
        ImGui::TextUnformatted("Disabled");
        ImGui::End();
        return;
    }

    ImGui::SameLine();
    if (ImGui::Button("Save Baseline"))
    {
        baseline_ms_ = average_ms_;
        for (size_t i = 0; i < kCounterCount; ++i)
        {
            baseline_counters_[i] = counters_[i].load(std::memory_order_relaxed);
        }
        baseline_memory_ = memory_;
        has_baseline_ = true;
    }
    if (has_baseline_)
    {
        ImGui::SameLine();
        if (ImGui::Button("Clear Baseline"))
        {
            has_baseline_ = false;
        }
    }

    ImGui::Text("Average over %.1f sec (ms / frame)", kAverageIntervalSec);

    const int column_count = has_baseline_ ? 4 : 2;
    if (ImGui::BeginTable("cpu_sections", column_count,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Current");
        if (has_baseline_)
        {
            ImGui::TableSetupColumn("Baseline");
            ImGui::TableSetupColumn("Diff");
        }
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < kSectionCount; ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(kSectionNames[i]);
            ImGui::TableNextColumn();
            ImGui::Text("%.3f", average_ms_[i]);
            if (has_baseline_)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%.3f", baseline_ms_[i]);
                ImGui::TableNextColumn();
                if (baseline_ms_[i] > 0.0f)
                {
                    const float ratio = (average_ms_[i] - baseline_ms_[i]) / baseline_ms_[i] * 100.0f;
                    const ImVec4 color = ratio <= 0.0f ? ImVec4(0.4f, 1.0f, 0.4f, 1.0f)
                                                       : ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    ImGui::TextColored(color, "%+.1f%%", ratio);
                }
                else
                {
                    ImGui::TextUnformatted("-");
                }
            }
        }
        ImGui::EndTable();
    }

    if (ImGui::BeginTable("cpu_counters", has_baseline_ ? 3 : 2,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Counter");
        ImGui::TableSetupColumn("Current");
        if (has_baseline_)
        {
            ImGui::TableSetupColumn("Baseline");
        }
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < kCounterCount; ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(kCounterNames[i]);
            ImGui::TableNextColumn();
            ImGui::Text("%u", counters_[i].load(std::memory_order_relaxed));
            if (has_baseline_)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%u", baseline_counters_[i]);
            }
        }
        ImGui::EndTable();
    }

    if (ImGui::BeginTable("memory", has_baseline_ ? 4 : 2,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Memory (MB)");
        ImGui::TableSetupColumn("Current");
        if (has_baseline_)
        {
            ImGui::TableSetupColumn("Baseline");
            ImGui::TableSetupColumn("Diff");
        }
        ImGui::TableHeadersRow();

        const MemoryStats& b = baseline_memory_;
        MemoryRow("Working Set", memory_.working_set, b.working_set, has_baseline_);
        MemoryRow("Peak Working Set", memory_.peak_working_set, b.peak_working_set, has_baseline_);
        MemoryRow("Private Bytes", memory_.private_bytes, b.private_bytes, has_baseline_);
        if (adapter_ != nullptr)
        {
            MemoryRow("VRAM Usage", memory_.vram_usage, b.vram_usage, has_baseline_);
            MemoryRow("VRAM Budget", memory_.vram_budget, b.vram_budget, has_baseline_);
            MemoryRow("Shared Usage", memory_.shared_usage, b.shared_usage, has_baseline_);
            MemoryRow("Shared Budget", memory_.shared_budget, b.shared_budget, has_baseline_);
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

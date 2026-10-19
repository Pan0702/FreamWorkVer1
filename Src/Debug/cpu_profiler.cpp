#include "cpu_profiler.h"

#include <iterator>

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

    ImGui::End();
}

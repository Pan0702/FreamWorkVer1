#include "imgui_manager.h"

#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

bool ImGuiManager::Initialize(HWND hwnd, ID3D12Device* device, ID3D12CommandQueue* command_queue, int num_frames_in_flight)
{
        if (hwnd == nullptr || !IsWindow(hwnd)) {
            return false;
        }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //F‚ÌÝ’è
    ImGui::StyleColorsDark();
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap_desc.NumDescriptors = 64;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&srv_heap_));
    if (FAILED(hr))
    {
        return false;
    }
    srv_allocator_.Create(device, srv_heap_.Get());
    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = device;
    init_info.CommandQueue = command_queue;
    init_info.NumFramesInFlight =  num_frames_in_flight;
    init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    init_info.SrvDescriptorHeap = srv_heap_.Get();
    init_info.UserData = this;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
                                        D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
    {
        ImGuiManager* manager = static_cast<ImGuiManager*>(info->UserData);
        manager->srv_allocator_.Alloc(out_cpu_handle, out_gpu_handle);
    };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
                                       D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
    {
        ImGuiManager* manager = static_cast<ImGuiManager*>(info->UserData);
         manager->srv_allocator_.Free(cpu_handle, gpu_handle);
    };
    
    if (!ImGui_ImplWin32_Init(hwnd))
    {
        return false;
    }
    
    if (!ImGui_ImplDX12_Init(&init_info))
    {
        return false;
    }
    return true;
}

void ImGuiManager::BeginFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame(ID3D12GraphicsCommandList* command_list)
{
    ImGui::Render();
    ID3D12DescriptorHeap* heaps[] = { srv_heap_.Get() };
    command_list->SetDescriptorHeaps(1, heaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);
}

void ImGuiManager::Shutdown()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::ExampleDescriptorHeapAllocator::Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
{
    IM_ASSERT(Heap == nullptr && FreeIndices.empty());
    Heap = heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
    HeapType = desc.Type;
    HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
    HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
    HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
    FreeIndices.reserve(static_cast<int>(desc.NumDescriptors));
    for (int n = static_cast<int>(desc.NumDescriptors); n > 0; n--)
        FreeIndices.push_back(n - 1);
}

void ImGuiManager::ExampleDescriptorHeapAllocator::Destroy()
{
    Heap = nullptr;
    FreeIndices.clear();
}

void ImGuiManager::ExampleDescriptorHeapAllocator::Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu,
                                                         D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu)
{
    IM_ASSERT(FreeIndices.Size > 0);
    int idx = FreeIndices.back();
    FreeIndices.pop_back();
    out_cpu->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
    out_gpu->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
}

void ImGuiManager::ExampleDescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu,
                                                        D3D12_GPU_DESCRIPTOR_HANDLE gpu)
{
    int cpu_idx = static_cast<int>((cpu.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
    int gpu_idx = static_cast<int>((gpu.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
    IM_ASSERT(cpu_idx == gpu_idx);
    FreeIndices.push_back(cpu_idx);
}

#pragma once
#include "imgui.h"
#include "../../Platform/Window.h"

class ImGuiManager
{
public:
    bool Initialize(HWND hwnd, ID3D12Device* device, ID3D12CommandQueue* command_queue, int num_frames_in_flight);
    void BeginFrame();
    void EndFrame(ID3D12GraphicsCommandList* command_list);
    void Shutdown();

private:
    struct ExampleDescriptorHeapAllocator
    {
        ID3D12DescriptorHeap* Heap = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE HeapType =
            D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
        UINT HeapHandleIncrement;
        ImVector<int> FreeIndices;

        void Create(ID3D12Device* device,
                    ID3D12DescriptorHeap* heap);
        void Destroy();
        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu,
                   D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu);
        void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu,
                  D3D12_GPU_DESCRIPTOR_HANDLE gpu);
    };

    ComPtr<ID3D12DescriptorHeap> srv_heap_;
    ExampleDescriptorHeapAllocator srv_allocator_ = {};
};

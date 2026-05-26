#pragma once
#include "../Core/common.h"
#include "../Platform/Window.h"
#include  "descriptor_heap.h"

class SwapChain
{
public:
    bool Initialize(IDXGIFactory6* factory, ID3D12Device* device, ID3D12CommandQueue* queue,
                    HWND hwnd, uint32_t width, uint32_t height);
    void Present();
    ID3D12Resource* GetCurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const;
    bool Resize(uint32_t width, uint32_t height);

private:
    bool CreateSwapChain(IDXGIFactory6* factory, ID3D12CommandQueue* command_queue,
                         HWND hwnd, uint32_t width, uint32_t height);

    bool CreateRenderTargetViews(ID3D12Device* device);
    
    ComPtr<IDXGISwapChain4> swap_chain_;
    DescriptorHeap rtv_heap_ = {};
    ID3D12Device* device_ = nullptr;
    ComPtr<ID3D12Resource> render_targets_[kFrameCount];
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles_[kFrameCount] = {};

};

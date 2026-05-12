#pragma once
#include "../Core/common.h"
#include "../Platform/Window.h"

class SwapChain
{
public:
    bool Initialize(IDXGIFactory6* factory, ID3D12Device* device, ID3D12CommandQueue* queue,
                    HWND hwnd, uint32_t width, uint32_t height);
    void Present();
    ID3D12Resource* GetCurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const;

private:
    bool CreateSwapChain(IDXGIFactory6* factory, ID3D12CommandQueue* command_queue,
                         HWND hwnd, uint32_t width, uint32_t height);

    bool CreateRenderTargetViews(ID3D12Device* device);
    
    ComPtr<IDXGISwapChain4> swap_chain_;
    ComPtr<ID3D12DescriptorHeap> rtv_heap_;
    ComPtr<ID3D12Resource> render_targets_[kFrameCount];

    uint32_t rtv_descriptor_size_ = 0;
};

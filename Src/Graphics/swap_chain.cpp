#include "swap_chain.h"

bool SwapChain::Initialize(IDXGIFactory6* factory, ID3D12Device* device, ID3D12CommandQueue* queue, HWND hwnd,
                           uint32_t width, uint32_t height)
{
    if (!CreateSwapChain(factory, queue, hwnd, width, height))
    {
        MessageBox(NULL, L"Failed to create swap chain", L"Error", MB_OK);
        return false;
    }

    if (!CreateRenderTargetViews(device))
    {
        MessageBox(NULL, L"Failed to create render target views", L"Error", MB_OK);
        return false;
    }

    return true;
}

bool SwapChain::CreateSwapChain(IDXGIFactory6* factory, ID3D12CommandQueue* command_queue, HWND hwnd, uint32_t width,
                                uint32_t height)
{
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.Width = width;
    swap_chain_desc.Height = height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = kFrameCount;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGISwapChain1> swap_chain1;
    HRESULT hr = factory->CreateSwapChainForHwnd(
        command_queue,hwnd,&swap_chain_desc,nullptr,
        nullptr,&swap_chain1);
    if (FAILED(hr))
    {
        return false;
    }
    hr = swap_chain1.As(&swap_chain_);
    if (FAILED(hr))
    {
        return false;
    }
    return true;  
}

bool SwapChain::CreateRenderTargetViews(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.NumDescriptors = kFrameCount;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = device->CreateDescriptorHeap(
        &rtv_heap_desc,
        IID_PPV_ARGS(&rtv_heap_));

    if (FAILED(hr))
    {
        return false;
    }

    rtv_descriptor_size_ =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle =
        rtv_heap_->GetCPUDescriptorHandleForHeapStart();

    for (uint32_t i = 0; i < kFrameCount; ++i)
    {
        hr = swap_chain_->GetBuffer(i, IID_PPV_ARGS(&render_targets_[i]));
        if (FAILED(hr))
        {
            return false;
        }

        device->CreateRenderTargetView(
            render_targets_[i].Get(),
            nullptr,
            rtv_handle);

        rtv_handle.ptr += rtv_descriptor_size_;
    }

    return true;
}

void SwapChain::Present()
{
    swap_chain_->Present(1, 0);
}

ID3D12Resource* SwapChain::GetCurrentBackBuffer()
{
    uint32_t index = swap_chain_->GetCurrentBackBufferIndex();
    return render_targets_[index].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentRtvHandle()
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle =
        rtv_heap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += rtv_descriptor_size_ * swap_chain_->GetCurrentBackBufferIndex();
    return handle;
}

#pragma once
#include "descriptor_heap.h"
#include "../Core/common.h"

class DepthStencil
{
public:
    bool Initialize(ID3D12Device* device, uint32_t width, uint32_t height);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;
    static constexpr DXGI_FORMAT kFORMAT = DXGI_FORMAT_D32_FLOAT;
    bool Resize(uint32_t w, uint32_t h);

private:
    bool CreateResource(uint32_t w, uint32_t h);
    ComPtr<ID3D12Resource> depth_stencil_;
    DescriptorHeap heap_;
    ID3D12Device* device_ = nullptr;
};

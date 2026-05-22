#pragma once
#include "descriptor_heap.h"
#include "../Core/common.h"
class DepthStencil
{
public:
    bool Initialize(ID3D12Device* device,uint32_t width,uint32_t height);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;
    static constexpr DXGI_FORMAT kFORMAT = DXGI_FORMAT_D32_FLOAT;
    
    private:
    ComPtr<ID3D12Resource> depth_stencil_;
    DescriptorHeap heap_;
};

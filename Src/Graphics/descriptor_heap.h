#pragma once
#include "../Core/common.h"
class DescriptorHeap
{
public:
    bool Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity, bool shader_visible);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const;
    ID3D12DescriptorHeap* GetHeap() const;
    bool Allocate(UINT& out_index);

private:
    ComPtr<ID3D12DescriptorHeap> heap_;
    D3D12_DESCRIPTOR_HEAP_TYPE type_;
    UINT descriptor_size_;
    UINT capacity_;
    UINT used_count_;
    bool shader_visible_;
};

#include "descriptor_heap.h"

#include <cassert>

bool DescriptorHeap::Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity,
                                bool shader_visible)
{
    used_count_ = 0;
    capacity_ = capacity;
    type_ = type;
    shader_visible_ = shader_visible;
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.Type = type;
    heap_desc.NumDescriptors = capacity;
    heap_desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap_));
    if (FAILED(hr))
    {
        return false;
    }
    descriptor_size_ = device->GetDescriptorHandleIncrementSize(type);
    return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuHandle(UINT index) const
{
    assert(index < capacity_ && "Descriptor index out of range");
    D3D12_CPU_DESCRIPTOR_HANDLE handle = heap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += static_cast<SIZE_T>(descriptor_size_ * index);
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuHandle(UINT index) const
{
    assert(shader_visible_ && "GPU handle requested on non-shader-visible heap");
    assert(index < capacity_ && "Descriptor index out of range");
    D3D12_GPU_DESCRIPTOR_HANDLE handle = heap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += static_cast<SIZE_T>(descriptor_size_ * index);
    return handle;
}

ID3D12DescriptorHeap* DescriptorHeap::GetHeap() const
{
    return heap_.Get();
}

bool DescriptorHeap::Allocate(UINT& out_index)
{
    if (used_count_ >= capacity_)
    {
        assert(false && "Capacity Over");
        return false;
    }
    out_index = used_count_++;
    return true;
}

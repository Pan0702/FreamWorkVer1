#pragma once
#include "descriptor_heap.h"
#include "graphics_device.h"
#include "../Core/Math/my_math.h"


class ShadowMap
{
public:
    bool Initialize(ID3D12Device* device,uint32 size,DescriptorHeap* srv_heap);
    D3D12_CPU_DESCRIPTOR_HANDLE GetDRV() const;
    uint32 GetSrvIndex() const;
    ID3D12Resource* GetResource() const;
    uint32 GetSize() const;
private:
    bool Create();
    ComPtr<ID3D12Resource> texture_;
    ID3D12Device* device_ = nullptr;
    DescriptorHeap* srv_heap_ = nullptr;
    DescriptorHeap dsv_heap_ = {};
    uint32 srv_index_ = 0;
    uint32 size_ = 0;
};

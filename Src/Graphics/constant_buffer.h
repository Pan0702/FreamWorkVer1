#pragma once
#include "../Core/common.h"

class ConstantBuffer
{
public:
    bool Initialize(ID3D12Device* device, uint32_t size);
    void Update(const void* data, uint32_t size);
    D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    void* mapped_ = nullptr;
    uint32_t aligned_size_ = 0;
};

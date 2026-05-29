#pragma once
#include "../Core/common.h"

struct ConstantBufferAllocation
{
    void* cpu = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu = 0;
    uint32_t size = 0;
};

class ConstantBufferAllocator
{
public:
    ConstantBufferAllocator() = default;
    ~ConstantBufferAllocator();

    ConstantBufferAllocator(const ConstantBufferAllocator&) = delete;
    ConstantBufferAllocator& operator=(const ConstantBufferAllocator&) = delete;

    bool Initialize(ID3D12Device* device, uint32_t capacity);
    bool Allocate(uint32_t size, ConstantBufferAllocation* out_allocation);
    void Reset();

    uint32_t GetCapacity() const;
    uint32_t GetUsedSize() const;

private:
    static uint32_t AlignConstantBufferSize(uint32_t size);

    ComPtr<ID3D12Resource> buffer_;
    void* mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_base_ = 0;
    uint32_t capacity_ = 0;
    uint32_t offset_ = 0;
};

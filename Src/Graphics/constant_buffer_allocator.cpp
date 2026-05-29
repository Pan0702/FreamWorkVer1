#include "constant_buffer_allocator.h"

ConstantBufferAllocator::~ConstantBufferAllocator()
{
    if (buffer_ && mapped_ != nullptr)
    {
        buffer_->Unmap(0, nullptr);
        mapped_ = nullptr;
    }
}

bool ConstantBufferAllocator::Initialize(ID3D12Device* device, uint32_t capacity)
{
    capacity_ = AlignConstantBufferSize(capacity);
    offset_ = 0;

    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC buffer_desc = {};
    buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    buffer_desc.Alignment = 0;
    buffer_desc.Width = capacity_;
    buffer_desc.Height = 1;
    buffer_desc.DepthOrArraySize = 1;
    buffer_desc.MipLevels = 1;
    buffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    buffer_desc.SampleDesc.Count = 1;
    buffer_desc.SampleDesc.Quality = 0;
    buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    buffer_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
                                                 D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                 IID_PPV_ARGS(&buffer_));
    if (FAILED(hr))
    {
        return false;
    }

    D3D12_RANGE read_range = {0, 0};
    hr = buffer_->Map(0, &read_range, &mapped_);
    if (FAILED(hr))
    {
        return false;
    }

    gpu_base_ = buffer_->GetGPUVirtualAddress();
    return true;
}

bool ConstantBufferAllocator::Allocate(uint32_t size, ConstantBufferAllocation* out_allocation)
{
    if (out_allocation == nullptr)
    {
        return false;
    }

    const uint32_t aligned_size = AlignConstantBufferSize(size);
    if (offset_ + aligned_size > capacity_)
    {
        return false;
    }

    out_allocation->cpu = static_cast<uint8_t*>(mapped_) + offset_;
    out_allocation->gpu = gpu_base_ + offset_;
    out_allocation->size = aligned_size;
    offset_ += aligned_size;
    return true;
}

void ConstantBufferAllocator::Reset()
{
    offset_ = 0;
}

uint32_t ConstantBufferAllocator::GetCapacity() const
{
    return capacity_;
}

uint32_t ConstantBufferAllocator::GetUsedSize() const
{
    return offset_;
}

uint32_t ConstantBufferAllocator::AlignConstantBufferSize(uint32_t size)
{
    return (size + 255u) & ~255u;
}

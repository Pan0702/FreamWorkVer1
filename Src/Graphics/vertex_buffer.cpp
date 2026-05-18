#include "vertex_buffer.h"

bool VertexBuffer::Initialize(ID3D12Device* device, const void* vertex_data, uint32_t total_size, uint32_t stride)
{
    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC buffer_desc = {};
    buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    buffer_desc.Alignment = 0;
    buffer_desc.Width = total_size;
    buffer_desc.Height = 1;
    buffer_desc.DepthOrArraySize = 1;
    buffer_desc.MipLevels = 1;
    buffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    buffer_desc.SampleDesc.Count = 1;
    buffer_desc.SampleDesc.Quality = 0;
    buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    buffer_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc,
                                                 D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer_));
    if (FAILED(hr))
    {
        return false;
    }

    void* mapped = nullptr;
    D3D12_RANGE read_range = {0, 0};
    hr = buffer_->Map(0, &read_range, &mapped);
    if (FAILED(hr))
    {
        return false;
    }
    errno_t err = memcpy_s(mapped, total_size, vertex_data, total_size);
    buffer_->Unmap(0, nullptr);
    if (err != 0)
    {
        
        return false;
    }
    
    view_.BufferLocation = buffer_->GetGPUVirtualAddress();
    view_.SizeInBytes = total_size;
    view_.StrideInBytes = stride;
    return true;
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetVertexBufferView() const
{
    return view_;
}

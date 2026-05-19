#pragma once
#include <cstdint>
#include "../Core/common.h"

class IndexBuffer
{
public:
    bool Initialize(ID3D12Device* device, const void* index_data, uint32_t total_size, DXGI_FORMAT format);
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
    uint32_t GetIndexCount() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    D3D12_INDEX_BUFFER_VIEW view_ = {};
    uint32_t index_count_ = 0;
};

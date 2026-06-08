#pragma once
#include "../Core/common.h"

class VertexBuffer
{
public:
    bool Initialize(ID3D12Device* device, const void* vertex_data
                    , uint32_t total_size, uint32_t stride);

    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    D3D12_VERTEX_BUFFER_VIEW view_ = {};
};

#pragma once
#include <span>
#include <memory>
#include <vector>
#include "../Core/common.h"
class IndexBuffer;
class VertexBuffer;

struct VertexData
{
    const void* data;
    uint32_t total_size;
    uint32_t stride;   
};
struct IndexData
{
    const void* data;
    uint32_t total_size;
    DXGI_FORMAT format;
};

class Mesh
{
public:
    bool Create(ID3D12Device* device, VertexData vertex_data, IndexData index_data, std::span<const D3D12_INPUT_ELEMENT_DESC>);
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
    uint32_t GetIndexCount();
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout();
    
private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;
    uint32_t index_count_ = 0;
};

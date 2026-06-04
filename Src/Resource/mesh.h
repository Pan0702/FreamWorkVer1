#pragma once
#include <span>
#include <memory>
#include <vector>
#include <string>
#include "../Core/common.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/vertex_buffer.h"
#include "../Core/Math/vec4.h"

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
struct MeshMaterialDesc
{
    Vec4 base_color;
    std::wstring diffuse_texture_path;
};

struct SubMesh
{
    uint32_t index_start;
    uint32_t index_count;
    uint32_t material_slot;   
};
class Mesh
{
public:
    bool Create(ID3D12Device* device, VertexData vertex_data, IndexData index_data, std::span<const D3D12_INPUT_ELEMENT_DESC>);
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
    uint32_t GetIndexCount();
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout();
    void SetMaterialDescs(std::span<const MeshMaterialDesc> material_descs);
    void SetSubMeshes(std::span<const SubMesh> sub_meshes);
    const std::vector<MeshMaterialDesc>& GetMaterialDecs(); 
    const std::vector<SubMesh>& GetSubMeshes();
private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;
    std::vector<MeshMaterialDesc> material_descs_;
    std::vector<SubMesh> sub_meshes_;
    uint32_t index_count_ = 0;
};

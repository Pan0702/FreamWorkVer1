#include "mesh.h"

#include "vertex_types.h"
#include "../Graphics/vertex_buffer.h"
#include "../Graphics/index_buffer.h"

bool Mesh::Create(ID3D12Device* device, VertexData vertex_data,IndexData index_data,std::span<const D3D12_INPUT_ELEMENT_DESC> layout)
{
    
    vertex_buffer_ = std::make_unique<VertexBuffer>();
    if (!vertex_buffer_->Initialize(device, vertex_data.data, vertex_data.total_size,
                                    vertex_data.stride))
    {
        MessageBox(nullptr, L"Failed to create vertex buffer", L"Error", MB_OK);
        return false;
    }
    
    index_buffer_ = std::make_unique<IndexBuffer>();
    if (!index_buffer_->Initialize(device, index_data.data, index_data.total_size, index_data.format))
    {
        MessageBox(nullptr, L"Failed to create index buffer", L"Error", MB_OK);
        return false;
    }
    input_layout_.assign(layout.begin(), layout.end());
    uint32_t bytes_per_index = (index_data.format == DXGI_FORMAT_R16_UINT) ? 2 : 4;
    index_count_ = index_data.total_size / bytes_per_index;
    return true;
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView()
{
    return vertex_buffer_->GetVertexBufferView();
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView()
{
    return index_buffer_->GetIndexBufferView();
}

uint32_t Mesh::GetIndexCount()
{
    return index_count_;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& Mesh::GetInputLayout()
{
    return input_layout_;   
}

void Mesh::SetMaterialDescs(std::span<const MeshMaterialDesc> material_descs)
{
    material_descs_.assign(material_descs.begin(), material_descs.end());
}

void Mesh::SetSubMeshes(std::span<const SubMesh> sub_meshes)
{
    sub_meshes_.assign(sub_meshes.begin(), sub_meshes.end());
}

const std::vector<MeshMaterialDesc>& Mesh::GetMaterialDecs()
{
    return material_descs_;  
}

const std::vector<SubMesh>& Mesh::GetSubMeshes()
{
    return sub_meshes_;
}

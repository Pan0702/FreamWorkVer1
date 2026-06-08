#include "skeltal_mesh.h"

#include "skeleton.h"

SkeletalMesh::SkeletalMesh() = default;

SkeletalMesh::~SkeletalMesh() = default;

bool SkeletalMesh::Create(ID3D12Device* device, VertexData vertex_data, IndexData index_data,
                          std::span<const D3D12_INPUT_ELEMENT_DESC> layout)
{
    vertex_buffer_ = std::make_unique<VertexBuffer>();
    if (!vertex_buffer_->Initialize(device, vertex_data.data, vertex_data.total_size, vertex_data.stride))
    {
        return false;
    }
    index_buffer_ = std::make_unique<IndexBuffer>();
    if (!index_buffer_->Initialize(device, index_data.data, index_data.total_size, index_data.format))
    {
        return false;
    }
    input_layout_.assign(layout.begin(),layout.end());
    uint32 byters_per_index = (index_data.format == DXGI_FORMAT_R16_UINT) ? 2 : 4;
    index_count_ = index_data.total_size / byters_per_index;
    return true;
}

D3D12_VERTEX_BUFFER_VIEW SkeletalMesh::GetVertexBufferView()
{
    return vertex_buffer_->GetVertexBufferView();
}

D3D12_INDEX_BUFFER_VIEW SkeletalMesh::GetIndexBufferView()
{
    return index_buffer_->GetIndexBufferView();
}

uint32_t SkeletalMesh::GetIndexCount()
{
    return index_count_;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& SkeletalMesh::GetInputLayout()
{
    return input_layout_;  
}

void SkeletalMesh::SetMaterialDescs(std::span<const MeshMaterialDesc> material_descs)
{
    material_descs_.assign(material_descs.begin(), material_descs.end());
}

void SkeletalMesh::SetSubMeshes(std::span<const SubMesh> sub_meshes)
{
    sub_meshes_.assign(sub_meshes.begin(), sub_meshes.end());
}

void SkeletalMesh::SetSkeleton(std::unique_ptr<Skeleton> sk)
{
    Skeleton_ = std::move(sk);
}

Skeleton* SkeletalMesh::GetSkeleton()
{
    return Skeleton_.get();
}

const std::vector<MeshMaterialDesc>& SkeletalMesh::GetMaterialDecs()
{
    return material_descs_; 
}

const std::vector<SubMesh>& SkeletalMesh::GetSubMeshes()
{
    return sub_meshes_;
}

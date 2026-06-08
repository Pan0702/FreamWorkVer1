#pragma once
#include <span>
#include <memory>
#include <vector>
#include <string>
#include "../Core/common.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/vertex_buffer.h"
#include "../Core/Math/vec4.h"
#include "mesh_data.h"

struct SkeletonNode;
class Skeleton;

class SkeletalMesh
{
public:
    SkeletalMesh() ;
    ~SkeletalMesh();
    bool Create(ID3D12Device* device,
        
    VertexData vertex_data, IndexData index_data, std::span<const D3D12_INPUT_ELEMENT_DESC>);
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
    uint32_t GetIndexCount();
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout();
    void SetMaterialDescs(std::span<const MeshMaterialDesc> material_descs);
    void SetSubMeshes(std::span<const SubMesh> sub_meshes);
    void SetSkeleton(std::unique_ptr<Skeleton> sk);
    Skeleton* GetSkeleton();
    const std::vector<MeshMaterialDesc>& GetMaterialDecs(); 
    const std::vector<SubMesh>& GetSubMeshes();

private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;
    std::vector<MeshMaterialDesc> material_descs_;
    std::vector<SubMesh> sub_meshes_;
    uint32_t index_count_ = 0;
    std::unique_ptr<Skeleton> Skeleton_;
};

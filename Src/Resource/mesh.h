#pragma once
#include <span>
#include <memory>
#include <vector>
#include "mesh_data.h"
#include "../Core/common.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/vertex_buffer.h"
#include "../Core/Math/my_math.h"
/**
 * @brief Meshのデータと処理をまとめる型。
 */
class Mesh
{
public:
    
    /**
     * @brief DirectX 12 リソースを作成する関数。
     * @param device DirectX 12 デバイス。
     * @param vertex_data 頂点バッファ作成に使うデータ。
     * @param index_data インデックスバッファ作成に使うデータ。
     * @param D3D12_INPUT_ELEMENT_DESC 引数。
     * @return 条件を満たす場合は true。
     */
    bool Create(ID3D12Device* device, VertexData vertex_data, IndexData index_data,
                std::span<const D3D12_INPUT_ELEMENT_DESC>);
    /**
     * @brief VertexBufferViewを取得する関数。
     * @return 戻り値。
     */
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
    /**
     * @brief IndexBufferViewを取得する関数。
     * @return 戻り値。
     */
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
    /**
     * @brief IndexCountを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetIndexCount();
    /**
     * @brief InputLayoutを取得する関数。
     * @return 戻り値。
     */
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout();
    /**
     * @brief MaterialDescsを設定する関数。
     * @param material_descs 引数。
     */
    void SetMaterialDescs(std::span<const MeshMaterialDesc> material_descs);
    /**
     * @brief SubMeshesを設定する関数。
     * @param sub_meshes 引数。
     */
    void SetSubMeshes(std::span<const SubMesh> sub_meshes);
    /**
     * @brief MaterialDecsを取得する関数。
     * @return 戻り値。
     */
    const std::vector<MeshMaterialDesc>& GetMaterialDecs();
    /**
     * @brief SubMeshesを取得する関数。
     * @return 戻り値。
     */
    const std::vector<SubMesh>& GetSubMeshes();

    /**
     * @brief 
     * @return collision_positionsを返す。
     */
    const std::vector<Vec3>& GetCollisionPositions() const;

    /**
    * @brief 
    * @return collision_indicesを返す。
    */
    const std::vector<uint32_t>& GetCollisionIndices() const;

    void SetCollisionMesh(std::vector<Vec3> positions, std::vector<uint32> indices);
private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;
    std::vector<MeshMaterialDesc> material_descs_;
    std::vector<SubMesh> sub_meshes_;
    std::vector<Vec3> collision_positions_;
    std::vector<uint32_t> collision_indices_;
    uint32_t index_count_ = 0;
};



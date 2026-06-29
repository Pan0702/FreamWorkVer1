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
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @param vertex_data アップロードする頂点データ。
     * @param index_data アップロードするインデックスデータ。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool Create(ID3D12Device* device, VertexData vertex_data, IndexData index_data,
                std::span<const D3D12_INPUT_ELEMENT_DESC>);
    /**
     * @brief 頂点バッファビュー を取得する。
     * @return 現在保持している 頂点バッファビュー。
     */
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
    /**
     * @brief インデックスバッファビュー を取得する。
     * @return 現在保持している インデックスバッファビュー。
     */
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
    /**
     * @brief インデックス数を取得する。
     * @return 描画に使うインデックス数。
     */
    uint32_t GetIndexCount();
    /**
     * @brief Input Layout を取得する。
     * @return 保持している Input Layout への参照。
     */
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout();
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param material_descs 描画に使用するマテリアル。
     */
    void SetMaterialDescs(std::span<const MeshMaterialDesc> material_descs);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param sub_meshes 描画単位に分割されたサブメッシュ配列。
     */
    void SetSubMeshes(std::span<const SubMesh> sub_meshes);
    /**
     * @brief Material Decs を取得する。
     * @return 保持している Material Decs への参照。
     */
    const std::vector<MeshMaterialDesc>& GetMaterialDecs();
    /**
     * @brief Sub Meshes を取得する。
     * @return 保持している Sub Meshes への参照。
     */
    const std::vector<SubMesh>& GetSubMeshes();

    /**
     * @brief Vec3 値 を取得する。
     * @return 2 点間の距離の二乗。
     */
    const std::vector<Vec3>& GetCollisionPositions() const;

    /**
     * @brief インデックス数を取得する。
     * @return 保持している 数値 への参照。
     */
    const std::vector<uint32_t>& GetCollisionIndices() const;

    /**
     * @brief 指定された値を内部状態に反映する。
     * @param positions 頂点位置の配列。
     * @param indices メッシュを構成するインデックス配列。
     */
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

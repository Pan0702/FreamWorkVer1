#pragma once
#include <span>
#include <memory>
#include <vector>
#include <string>
#include "../Core/common.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/vertex_buffer.h"
#include "mesh_data.h"
#include "../Core/Math/intersect.h"

struct SkeletonNode;

class Skeleton;
/**
 * @brief SkeletalMeshのデータと処理をまとめる型。
 */
class SkeletalMesh
{
public:
    /**
     * @brief 値を初期化する。
     */
    SkeletalMesh();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~SkeletalMesh();
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
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
    /**
     * @brief インデックスバッファビュー を取得する。
     * @return 現在保持している インデックスバッファビュー。
     */
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
    /**
     * @brief インデックス数を取得する。
     * @return 描画に使うインデックス数。
     */
    uint32_t GetIndexCount() const;
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
     * @brief 指定された値を内部状態に反映する。
     * @param sk 関連付けるスケルトン。
     */
    void SetSkeleton(std::unique_ptr<Skeleton> sk);
    /**
     * @brief スケルトン を取得する。
     * @return スケルトン。見つからない、または未作成の場合は nullptr。
     */
    Skeleton* GetSkeleton() const;
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
     * @brief スケルタルメッシュの境界ボックスを取得する。
     * @return メッシュ全体を覆う境界ボックス。
     */
    const Box& GetBounds() const;

private:
    std::unique_ptr<VertexBuffer> vertex_buffer_;
    std::unique_ptr<IndexBuffer> index_buffer_;
    std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout_;
    std::vector<MeshMaterialDesc> material_descs_;
    std::vector<SubMesh> sub_meshes_;
    uint32_t index_count_ = 0;
    std::unique_ptr<Skeleton> Skeleton_;
    Box bounds_;
};

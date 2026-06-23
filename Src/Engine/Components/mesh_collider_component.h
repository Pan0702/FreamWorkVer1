#pragma once
#include "collider_component.h"
#include <vector>
class Mesh;

// Actor に追加して使う MeshColliderComponent の状態と処理をまとめる。
class MeshColliderComponent : public ColliderComponent
{
public:
    /**
     * @brief インスタンスの初期状態を整える。
     * @param mesh 読み込み、描画、または判定に使用するメッシュ。
     */
    explicit MeshColliderComponent(const Mesh* mesh = nullptr);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param mesh 読み込み、描画、または判定に使用するメッシュ。
     */
    void SetMesh(const Mesh* mesh = nullptr);
    /**
     * @brief コライダーの形状種別を取得する。
     * @return 現在保持している コライダーの形状種別。
     */
    ColliderShape GetColliderShape() const override;
    /**
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param vertices vertices に設定する値。
     * @param indices indices に設定する値。
     */
    void SetTriangles(std::vector<Vec3> vertices,std::vector<uint32> indices);
    /**
     * @brief Vec3 値 を取得する。
     * @return 保持している Vec3 値 への参照。
     */
    const std::vector<Vec3>& GetVertices() const;
    /**
     * @brief 数値 を取得する。
     * @return 保持している 数値 への参照。
     */
    const std::vector<uint32>& GetIndices() const;
    /**
     * @brief ワールド行列 を取得する。
     * @return 現在の Transform から作成したワールド行列。
     */
    Mat GetWorldMatrix() const;

private:
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void DrawDebug() const override;
    std::vector<Vec3> vertices_;
    std::vector<uint32> indices_;
};

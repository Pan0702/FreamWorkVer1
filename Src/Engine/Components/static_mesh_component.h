#pragma once
#include <cstdint>

#include "../component.h"

class MaterialSlot;
class Material;
class Mesh;
class MeshRenderer;

/**
 * @brief StaticMeshComponentのデータと処理をまとめる型。
 */
class StaticMeshComponent : public Component
{
public:
    /**
     * @brief 値を初期化する。
     */
    StaticMeshComponent() = default;
    /**
     * @brief 値を初期化する。
     * @param mesh 読み込み、描画、または判定に使用するメッシュ。
     * @param material_slot 描画に使用するマテリアル。
     */
    StaticMeshComponent(Mesh* mesh, MaterialSlot* material_slot);
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~StaticMeshComponent() override;

    /**
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 共有コンテキストから登録を外し、システム側の参照を切る。
     */
    void OnDetach() override;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param mesh 読み込み、描画、または判定に使用するメッシュ。
     */
    void SetMesh(Mesh* mesh);
    /**
     * @brief メッシュ を取得する。
     * @return メッシュ。見つからない、または未作成の場合は nullptr。
     */
    Mesh* GetMesh() const;
    /**
     * @brief マテリアルスロット を取得する。
     * @return マテリアルスロット。見つからない、または未作成の場合は nullptr。
     */
    MaterialSlot* GetMaterialSlot() const;
    
    int sort_key = 0;

private:
    Mesh* mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    MeshRenderer* renderer_ = nullptr;
};

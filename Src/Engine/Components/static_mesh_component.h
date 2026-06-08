#pragma once
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
    StaticMeshComponent() = default;
    /**
     * @brief StaticMeshComponentを初期化するコンストラクタ。
     * @param mesh 引数。
     * @param material_slot 引数。
     */
    StaticMeshComponent(Mesh* mesh, MaterialSlot* material_slot);
    ~StaticMeshComponent() override;

    /**
     * @brief OnAttachを行う関数。
     * @param context 共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief OnDetachを行う関数。
     */
    void OnDetach() override;
    /**
     * @brief Meshを設定する関数。
     * @param mesh 引数。
     */
    void SetMesh(Mesh* mesh);
    /**
     * @brief Meshを取得する関数。
     * @return 戻り値。
     */
    Mesh* GetMesh() const;
    /**
     * @brief MaterialSlotを取得する関数。
     * @return 戻り値。
     */
    MaterialSlot* GetMaterialSlot() const;

private:
    Mesh* mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    MeshRenderer* renderer_ = nullptr;
};

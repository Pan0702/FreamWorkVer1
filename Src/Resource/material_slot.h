#pragma once
#include <vector>

#include "material.h"
/**
 * @brief MaterialSlotのデータと処理をまとめる型。
 */
class MaterialSlot
{
public:
    /**
     * @brief 値を初期化する。
     * @param materials 描画に使用するマテリアル。
     */
    MaterialSlot(const std::vector<MeshMaterialDesc>& materials);
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~MaterialSlot();
    /**
     * @brief マテリアル を取得する。
     * @param index 参照する要素番号。
     * @return マテリアル。見つからない、または未作成の場合は nullptr。
     */
    Material* GetMaterial(uint32_t index) const;
private:
    std::vector<std::unique_ptr<Material>> materials_;
};

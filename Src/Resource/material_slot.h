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
     * @brief MaterialSlotを初期化するコンストラクタ。
     * @param materials 引数。
     */
    MaterialSlot(const std::vector<MeshMaterialDesc>& materials);
    /**
     * @brief MaterialSlotの終了処理を行うデストラクタ。
     */
    ~MaterialSlot();
    /**
     * @brief Materialを取得する関数。
     * @param index 参照する番号。
     * @return 戻り値。
     */
    Material* GetMaterial(uint32_t index) const;
private:
    std::vector<std::unique_ptr<Material>> materials_;
};

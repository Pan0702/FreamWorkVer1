#pragma once
#include <memory>
#include "../Engine/level_base.h"
#include "../Resource/material.h"
#include "../Resource/material_slot.h"  // MaterialSlot の完全な定義を使うために必要。

class MaterialSlot;

/**
 * @brief TestLevelのデータと処理をまとめる型。
 */
class TestLevel : public LevelBase
{
public:
    /**
     * @brief TestLevelを初期化するコンストラクタ。
     */
    TestLevel();
    ~TestLevel() override;
    /**
     * @brief OnEnterを行う関数。
     */
    void OnEnter() override;
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    void Tick(float dt) override;
private:
    std::unique_ptr<MaterialSlot> material_slot_;
    std::unique_ptr<MaterialSlot> material_slot_2_;
    std::unique_ptr<MaterialSlot> skeletal_material_slot_;
    class TransformComponent* collider_test_transform_a_ = nullptr; //固定側の球
    class TransformComponent* collider_test_transform_b_ = nullptr; //動かす側の球
    float collider_test_radius_a_ = 1.0f;
    float collider_test_radius_b_ = 1.0f;
};

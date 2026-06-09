#pragma once
#include <memory>
#include "../Engine/level_base.h"
#include "../Core/Math/my_math.h"
#include "../Resource/material.h"
#include "../Resource/material_slot.h"  // MaterialSlot �̊��S�Ȓ�`���g�����߂ɕK�v�B

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
    class TransformComponent* collider_test_transform_box_ = nullptr; //固定側のBox
    class TransformComponent* collider_test_transform_b_ = nullptr;   //動かす側の球
    class MeshColliderComponent* collider_test_mesh_ = nullptr;       //固定側のMesh(壁quad)
    Vec3 collider_test_box_half_ = {1.0f, 1.0f, 1.0f};                //Boxのローカル半幅
    float collider_test_radius_b_ = 1.0f;
};

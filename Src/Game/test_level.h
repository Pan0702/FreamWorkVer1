#pragma once
#include <memory>
#include "../Engine/level_base.h"
#include "../Core/Math/my_math.h"
#include "../Resource/material.h"
#include "../Resource/material_slot.h"  
class MaterialSlot;

/**
 * @brief TestLevelのデータと処理をまとめる型。
 */
class TestLevel : public LevelBase
{
public:
    /**
     * @brief インスタンスの初期状態を整える。
     */
    TestLevel();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~TestLevel() override;
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void OnEnter() override;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
private:
    std::unique_ptr<MaterialSlot> material_slot_;
    std::unique_ptr<MaterialSlot> material_slot_2_;
    std::unique_ptr<MaterialSlot> skeletal_material_slot_;
    class TransformComponent* collider_test_transform_box_ = nullptr; 
    // Actor に追加して使う TransformComponent の状態と処理をまとめる。
    class TransformComponent* collider_test_transform_b_ = nullptr;   //動かす側の球
    // Actor に追加して使う MeshColliderComponent の状態と処理をまとめる。
    class MeshColliderComponent* collider_test_mesh_ = nullptr;       //固定側のMesh(壁quad)
    Vec3 collider_test_box_half_ = {1.0f, 1.0f, 1.0f};                //Boxのローカル半幅
    float collider_test_radius_b_ = 1.0f;
};

#pragma once
#include "collider_component.h"

struct Sphere;

// Actor に追加して使う SphereColliderComponent の状態と処理をまとめる。
/**
 * @brief 球形状の衝突判定を提供するコンポーネント。
 */
class SphereColliderComponent : public ColliderComponent
{
public:
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param radius 形状の半径。
     */
    void SetRadius(float radius);
    /**
     * @brief 半径 を取得する。
     * @return 現在保持している 半径。
     */
    float GetRadius() const;
    /**
     * @brief コライダーの形状種別を取得する。
     * @return 現在保持している コライダーの形状種別。
     */
    ColliderShape GetColliderShape() const override;
    /**
     * @brief Collider Shape Data を取得する。
     * @return 現在保持している Collider Shape Data。
     */
    Sphere GetColliderShapeData() const;

private:
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void DrawDebug() const override;
    float local_radius_ = 0.0f;
};

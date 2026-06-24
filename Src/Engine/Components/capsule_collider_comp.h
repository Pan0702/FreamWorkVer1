#pragma once
#include "collider_component.h"

struct Capsule;
class CapsuleColliderComponent : public ColliderComponent
{
public:
    /**
  * @brief 指定された値を内部状態に反映する。
  * @param size size に設定する値。
  */
    void SetHalfSize(const Vec3& size);
    /**
     * @brief Vec3 値 を取得する。
     * @return 保持している Vec3 値 への参照。
     */
    const Vec3& GetHalfSize() const;
    /**
     * @brief コライダーの形状種別を取得する。
     * @return 現在保持している コライダーの形状種別。
     */
    ColliderShape GetColliderShape() const override;
    /**
     * @brief Collider Box Data を取得する。
     * @return 現在保持している Collider Box Data。
     */
    Capsule GetColliderCapsuleData() const;

private:
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void DrawDebug() const override;
    Vec3 half_size = {0,0,0};
};

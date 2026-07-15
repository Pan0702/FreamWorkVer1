#pragma once
#include "collider_component.h"
struct Box;
// Actor に追加して使う BoxColliderComponent の状態と処理をまとめる。
/**
 * @brief ボックス形状の衝突判定を提供するコンポーネント。
 */
class BoxColliderComponent : public ColliderComponent
{
public:
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param size 設定するサイズ。
     */
    void SetHalfSize(const Vec3& size);
    /**
     * @brief Vec3 値 を取得する。
     * @return 2 点間の距離の二乗。
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
    Box GetColliderBoxData() const;

private:
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void DrawDebug() const override;
    Vec3 half_size = {0,0,0};
};

#pragma once
#include "collider_component.h"

class SkeletalMesh;
struct Capsule;

// スケルタルメッシュに合わせたカプセルコライダーを扱うコンポーネント。//
/**
 * @brief カプセル形状の衝突判定を提供するコンポーネント。
 */
class CapsuleColliderComponent : public ColliderComponent
{
public:
    /**
     * @brief メッシュ情報をもとにカプセルコライダーを作成する。
     * @param mesh カプセルサイズの基準に使うスケルタルメッシュ。
     */
    explicit CapsuleColliderComponent(const SkeletalMesh* mesh = nullptr);
    /**
     * @brief カプセルの半径と高さの元になる半サイズを設定する。
     * @param size XZ を半径、Y を高さ方向の基準として使う半サイズ。
     */
    void SetHalfSize(const Vec3& size);
    /**
     * @brief カプセル計算に使う半サイズを取得する。
     * @return 現在設定されている半サイズ。
     */
    const Vec3& GetHalfSize() const;
    /**
     * @brief このコライダーがカプセル形状であることを返す。
     * @return ColliderShape::kCapsule。
     */
    ColliderShape GetColliderShape() const override;
    /**
     * @brief ワールド判定に渡すカプセル形状データを作成する。
     * @return 現在の Transform と半サイズから作った Capsule。
     */
    Capsule GetColliderCapsuleData() const;

private:
    /**
     * @brief 現在のカプセル形状をデバッグラインとして描画する。
     */
    void DrawDebug() const override;
    Vec3 half_size = {0,0,0};
    Vec3 center_offset_ = {0,0,0};
};
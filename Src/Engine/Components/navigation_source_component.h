#pragma once
#include "../component.h"
#include "../../Core/Math/my_math.h"
#include "../Navigation/navigation_geometry.h"
class NavigationSystem;
class MeshColliderComponent;
struct NavigationGeometry;

/**
 * @brief 所有アクターの形状を、NavMesh の元になる地形として登録するコンポーネント。
 *
 * 取り付けと同時に NavigationSystem へ登録し、取り外しで解除する。
 * 形状は MeshColliderComponent が持つものをそのまま使う。
 */
class NavigationSourceComponent : public Component
{
public:
    /**
     * @brief 地形の形状を提供するコライダーを設定する。
     * @param mesh 形状の元になるメッシュコライダー。
     */
    explicit NavigationSourceComponent(MeshColliderComponent* mesh);
    /**
     * @brief 所属する World の NavigationSystem へ自身を登録する。
     * @param context 取り付け時に渡される参照情報。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief NavigationSystem への登録を解除する。
     */
    void OnDetach() override;
    /**
     * @brief NavMesh 構築に使う形状を取り出す。
     * @return メッシュコライダーの頂点と、ワールド空間での配置。
     */
    NavigationGeometry GetGeometry() const;

private:
    MeshColliderComponent* mesh_;
    NavigationSystem* navigation_system_ = nullptr;
    uint32 source_id_                    = 0;
};

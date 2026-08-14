#pragma once
#include <vector>

#include "navigation_config.h"
#include "../../Core/Math/my_math.h"
#include "navigation_geometry.h"
#include "navigation_type.h"

class NavigationSourceComponent;

/**
 * @brief World が保持する NavMesh の生成と経路探索の窓口。
 *
 * NavMesh の元になる地形は NavigationSourceComponent が登録する。
 * 登録された地形から Rebuild で NavMesh を構築し、以降の FindPath に使う。
 */
class NavigationSystem
{
public:
    /**
     * @brief NavMesh の元になる地形を登録する。
     * @param component 地形を提供するコンポーネント。
     * @return 登録を識別する ID。解除時に使う。
     */
    uint32 RegisterSource(NavigationSourceComponent* component);
    /**
     * @brief 登録済みの地形を解除する。
     * @param source_id RegisterSource が返した ID。
     */
    void UnregisterSource(uint32 source_id);
    /**
     * @brief 登録されている全ての地形の形状を集める。
     * @return NavMesh 構築に使う形状の一覧。
     */
    std::vector<NavigationGeometry> CollectGeometries() const;
    /**
     * @brief 登録されている地形から NavMesh を作り直す。
     * @param config ボクセル解像度やエージェント寸法などの構築パラメータ。
     * @return 構築に成功した場合は true。
     */
    bool Rebuild(const NavigationConfig& config);
    /**
     * @brief NavMesh 上で 2 点間の経路を求める。
     * @param start_position 出発地点のワールド座標。
     * @param goal_position 目的地のワールド座標。
     * @param out_path 求めた経路の通過点。
     * @param out_polygon_path 経路が通ったポリゴンの列。不要なら nullptr。
     * @return 経路が見つかった場合は true。
     */
    bool FindPath(const Vec3& start_position, const Vec3& goal_position,
                  std::vector<Vec3>& out_path,
                  std::vector<uint32>* out_polygon_path = nullptr) const;
    /**
     * @brief 高さ方向の追従に使う詳細メッシュを取得する。
     * @return 保持している詳細メッシュ。
     */
    const NavigationDetailMeshData& GetDetailMeshData() const;
    /**
     * @brief 経路探索に使うポリゴンメッシュを取得する。
     * @return 保持している NavMesh。
     */
    const NavigationMeshData& GetMeshData() const;
    /**
     * @brief NavMesh の形状をデバッグ描画する。
     */
    void DrawDebug() const;
    /**
     * @brief NavMesh のデバッグ描画の有効無効を設定する。
     * @param enabled 描画する場合は true。
     */
    void SetDebugDrawEnabled(bool enabled) { debug_draw_enabled_ = enabled; }
    /**
     * @brief NavMesh のデバッグ描画が有効かどうかを取得する。
     * @return 有効な場合は true。
     */
    bool IsDebugDrawEnabled() const { return debug_draw_enabled_; }
private:
    /**
     * @brief 登録された地形と、解除に使う ID の組。
     */
    struct RegisteredSource
    {
        NavigationSourceComponent* component = nullptr;
        uint32 id = 0;
    };

    bool debug_draw_enabled_ = false;
    std::vector<RegisteredSource> sources_;
    uint32 next_id_ = 1;
    NavigationMeshData mesh_data_;
    NavigationDetailMeshData detail_mesh_data_;
};

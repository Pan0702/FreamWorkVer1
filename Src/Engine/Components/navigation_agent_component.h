#pragma once

#include <vector>

#include "../component.h"
#include "../../Core/Math/my_math.h"

class NavigationSystem;

/**
 * @brief アクターに NavMesh 上の経路探索と経路保持の機能を与えるコンポーネント。
 *
 * 目的地を設定すると所有アクターの現在位置から経路を求めて保持する。
 * 経路に沿った移動そのものは行わないため、追従は利用側(Controller など)が
 * ウェイポイントを取り出して行う。
 */
class NavigationAgentComponent : public Component
{
public:
    /**
     * @brief 所属する World から NavigationSystem を取得する。
     * @param context 取り付け時に渡される参照情報。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 保持している経路と参照を解放する。
     */
    void OnDetach() override;
    /**
     * @brief デバッグ描画を 1 フレーム分更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;

    /**
     * @brief 所有アクターの現在位置から目的地までの経路を求めて保持する。
     * @param destination 目的地のワールド座標。
     * @return 経路が見つかった場合は true。
     */
    bool SetDestination(const Vec3& destination);
    /**
     * @brief 保持している経路を破棄する。
     */
    void ClearPath();

    /**
     * @brief 保持している経路を取得する。
     * @return 経路の通過点。経路がない場合は空。
     */
    const std::vector<Vec3>& GetPath() const;
    /**
     * @brief 経路探索が通ったポリゴン列を取得する。
     * @return コリドーを構成するポリゴンのインデックス列。
     */
    const std::vector<uint32>& GetCorridorPolygonIndices() const;
    /**
     * @brief 次に目指すウェイポイントの位置を取得する。
     * @return 経路上のインデックス。経路を走破済みの場合は経路の要素数と等しい。
     */
    uint32 GetCurrentPathIndex() const;
    /**
     * @brief 経路を保持しているかどうかを調べる。
     * @return 経路がある場合は true。
     */
    bool HasPath() const;

    /**
     * @brief 経路のデバッグ描画の有効無効を設定する。
     * @param enabled 描画する場合は true。
     */
    void SetDebugDrawEnabled(bool enabled);
    /**
     * @brief 経路のデバッグ描画が有効かどうかを取得する。
     * @return 有効な場合は true。
     */
    bool IsDebugDrawEnabled() const;
    /**
     * @brief 経路探索が通ったポリゴンのデバッグ描画の有効無効を設定する。
     * @param enabled 描画する場合は true。
     */
    void SetDebugDrawCorridorEnabled(bool enabled);
    /**
     * @brief コリドーのデバッグ描画が有効かどうかを取得する。
     * @return 有効な場合は true。
     */
    bool IsDebugDrawCorridorEnabled() const;
    /**
     * @brief 経路線のデバッグ描画に使う色を設定する。
     * @param color 描画色。
     */
    void SetDebugDrawColor(const Vec4& color);
    /**
     * @brief デバッグ描画を地面から浮かせる高さを設定する。
     * @param height_offset 地形と重なってちらつくのを防ぐための高さ。
     */
    void SetDebugDrawHeightOffset(float height_offset);
    /**
     * @brief 経路の始点と終点に描く球の半径を設定する。
     * @param radius 球の半径。
     */
    void SetDebugPointRadius(float radius);
    /**
     * @brief 次に目指すウェイポイントを取得する。
     * @param out_waypoint 取得したウェイポイントの位置。
     * @return ウェイポイントがある場合は true。経路がない、または走破済みの場合は false。
     */
    bool TryGetCurrentWaypoint(Vec3& out_waypoint) const;
    /**
     * @brief 指定位置がウェイポイントに到達していれば、次のウェイポイントへ進める。
     *
     * 判定は XZ 平面で行う。近接したウェイポイントが続く場合はまとめて進める。
     * @param position 到達判定に使う現在位置。
     * @param radius 到達とみなす距離。
     */
    void AdvanceWaypointIfReached(const Vec3& position, float radius);

private:
    /**
     * @brief 保持している経路とコリドーを描画する。
     */
    void DrawDebug() const;

    NavigationSystem* navigation_system_ = nullptr;
    std::vector<Vec3> path_;
    std::vector<uint32> corridor_polygon_indices_;
    uint32 current_path_index_ = 0;

    bool debug_draw_enabled_ = false;
    bool debug_draw_corridor_enabled_ = false;
    Vec4 debug_draw_color_ = Vec4(1.0f, 0.0f, 1.0f, 1.0f);
    float debug_draw_height_offset_ = 2.0f;
    float debug_point_radius_ = 3.0f;
};

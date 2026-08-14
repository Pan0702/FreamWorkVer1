#pragma once
#include "navigation_type.h"
#include "../../Core/Math/my_math.h"

/**
 * @brief NavMesh 上の経路探索を行う。
 *
 * FindPath は A* で通過するポリゴン列(コリドー)を求め、ファンネル法で
 * コリドー内の折れ線に直し、最後に見通し判定で不要な曲がり角を削る、
 * という 3 段階で経路を作る。状態を持たないため使い捨てで構わない。
 */
class NavigationMeshQuery
{
public:
    /**
     * @brief 指定位置に最も近いポリゴンを探す。
     * @param mesh_data 対象の NavMesh。
     * @param position 探索の基準となる位置。
     * @param out_polygon_index 見つかったポリゴンのインデックス。
     * @return 見つかった場合は true。
     */
    bool FindNearestPolygon(const NavigationMeshData& mesh_data,
                            const Vec3& position, uint32& out_polygon_index) const;
    /**
     * @brief A* で始点ポリゴンから終点ポリゴンまでのポリゴン列を求める。
     * @param mesh_data 対象の NavMesh。
     * @param start_poly 始点のポリゴンインデックス。
     * @param goal_poly 終点のポリゴンインデックス。
     * @param start_position 始点の位置。コストの起点に使う。
     * @param goal_position 終点の位置。推定コストの計算に使う。
     * @param out_poly 求めたポリゴン列。
     * @return 経路が見つかった場合は true。
     */
    bool FindPolygonPath(const NavigationMeshData& mesh_data, uint32 start_poly, uint32 goal_poly,
                         const Vec3& start_position, const Vec3& goal_position,
                         std::vector<uint32>& out_poly) const;
    /**
     * @brief ファンネル法で、ポリゴン列を通る最短の折れ線を求める。
     * @param mesh_data 対象の NavMesh。
     * @param polygon_path 通過するポリゴン列。
     * @param start_pos 始点の位置。
     * @param goal_pos 終点の位置。
     * @param out_path 求めた折れ線の通過点。
     * @return 折れ線が作れた場合は true。
     */
    bool FindStraightPath(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                          const Vec3& start_pos, const Vec3& goal_pos, std::vector<Vec3>& out_path) const;
    /**
     * @brief 指定位置に最も近いポリゴンと、その上の最近接点を探す。
     * @param mesh_data 対象の NavMesh。
     * @param position 探索の基準となる位置。
     * @param out_polygon_index 見つかったポリゴンのインデックス。
     * @param out_closest_point ポリゴン上で最も近い点。
     * @return 見つかった場合は true。
     */
    bool FindNearestPolygon(const NavigationMeshData& mesh_data, const Vec3& position,
                            uint32& out_polygon_index, Vec3& out_closest_point) const;
    /**
     * @brief NavMesh 上で 2 点間の経路を求める。
     * @param mesh_data 対象の NavMesh。
     * @param start_pos 出発地点。
     * @param goal_position 目的地。
     * @param out_path 求めた経路の通過点。
     * @param out_polygon_path 経路探索が通ったポリゴン列。不要なら nullptr。
     * @return 経路が見つかった場合は true。
     */
    bool FindPath(const NavigationMeshData& mesh_data, const Vec3& start_pos, const Vec3& goal_position,
                  std::vector<Vec3>& out_path,
                  std::vector<uint32>* out_polygon_path = nullptr) const;

private:
    /**
     * @brief A* の探索でポリゴンが今どの集合にあるかを表す。
     */
    enum class State : uint8
    {
        kUnvisited,
        kOpen,
        kClosed,
    };

    /**
     * @brief A* がポリゴン 1 枚ごとに持つ探索状態。
     */
    struct Node
    {
        float g_cost = FLT_MAX;
        float h_cost = 0.0f;
        float f_cost = FLT_MAX;

        Vec3 position;
        uint32 parent_index = UINT32_MAX;
        State state = State::kUnvisited;
    };

    /**
     * @brief ファンネル法が通り抜ける門。隣接ポリゴンの共有辺に対応する。
     *
     * left と right は進行方向から見た左右の端点。
     */
    struct Portal
    {
        Vec3 left;
        Vec3 right;
    };

    /**
     * @brief ポリゴンの重心を求める。
     * @param mesh_data 対象の NavMesh。
     * @param polygon_index 対象のポリゴンインデックス。
     * @return 頂点を平均した位置。
     */
    Vec3 CalcPolygonCenter(const NavigationMeshData& mesh_data, uint32 polygon_index) const;
    /**
     * @brief 三角形上で指定点に最も近い点を求める。
     * @param point 基準となる点。
     * @param a 三角形の頂点。
     * @param b 三角形の頂点。
     * @param c 三角形の頂点。
     * @return 三角形上の最近接点。
     */
    Vec3 CalcClosestPointOnTriangle(const Vec3& point, const Vec3& a, const Vec3& b, const Vec3& c) const;
    /**
     * @brief 辺の上で、現在位置から目的地までの距離が最短になる通過点を求める。
     * @param current_position 現在位置。
     * @param goal_position 目的地。
     * @param edge_start 辺の始点。
     * @param edge_end 辺の終点。
     * @return 辺上の通過点。
     */
    Vec3 CalcBestPortalPoint(const Vec3& current_position, const Vec3& goal_position,
                             const Vec3& edge_start, const Vec3& edge_end) const;
    /**
     * @brief 2 つのポリゴンが共有する辺を求める。
     * @param mesh_data 対象の NavMesh。
     * @param first_polygon_index 一方のポリゴンインデックス。
     * @param second_polygon_index もう一方のポリゴンインデックス。
     * @param out_a 共有辺の始点。first_polygon_index 側の頂点順で返す。
     * @param out_b 共有辺の終点。
     * @return 共有辺がある場合は true。
     */
    bool TryGetSharedEdge(const NavigationMeshData& mesh_data, uint32 first_polygon_index,
                          uint32 second_polygon_index, Vec3& out_a, Vec3& out_b) const;
    /**
     * @brief ポリゴン列から、ファンネル法が使う門の列を作る。
     * @param mesh_data 対象の NavMesh。
     * @param polygon_path 通過するポリゴン列。
     * @param start_pos 始点の位置。幅を持たない門として先頭に入れる。
     * @param goal_pos 終点の位置。幅を持たない門として末尾に入れる。
     * @param out_portals 作成した門の列。
     * @return 作成に成功した場合は true。
     */
    bool BuildPortals(const NavigationMeshData& mesh_data, const std::vector<uint32>& polygon_path,
                      const Vec3& start_pos, const Vec3& goal_pos, std::vector<Portal>& out_portals) const;
    /**
     * @brief 2 点を結ぶ線分が NavMesh 上を直進できるかを、ポリゴンを辿って調べる。
     *
     * 交点がポリゴンの頂点に乗る場合に備え、その頂点を共有するポリゴンへも
     * 枝分かれして探索する。
     * @param mesh_data 対象の NavMesh。
     * @param start_polygon_index 始点が乗るポリゴンインデックス。
     * @param goal_polygon_index 終点が乗るポリゴンインデックス。
     * @param start_position 始点の位置。
     * @param goal_position 終点の位置。
     * @param out_path 始点から終点までの通過点。辺との交点を含む。
     * @param out_polygon_path 通過したポリゴン列。不要なら nullptr。
     * @return 直進できた場合は true。
     */
    bool TraceSegmentAcrossNavMesh(const NavigationMeshData& mesh_data,
                                   uint32 start_polygon_index, uint32 goal_polygon_index,
                                   const Vec3& start_position, const Vec3& goal_position,
                                   std::vector<Vec3>& out_path,
                                   std::vector<uint32>* out_polygon_path) const;
    /**
     * @brief 見通しの利く点まで一気に進めるように、経路の曲がり角を削る。
     *
     * ファンネル法はポリゴンの分割の仕方によって不要な曲がり角を作ることがある。
     * 経路の後ろ側から直進できる点を探し、間の点を捨てることでこれを取り除く。
     * @param mesh_data 対象の NavMesh。
     * @param path 削る前の経路。
     * @param polygon_path 経路が通るポリゴン列。
     * @param out_path 削った後の経路。
     * @param out_polygon_path 削った後の経路が通るポリゴン列。
     * @return 削減に成功した場合は true。
     */
    bool OptimizePathVisibility(const NavigationMeshData& mesh_data,
                                const std::vector<Vec3>& path,
                                const std::vector<uint32>& polygon_path,
                                std::vector<Vec3>& out_path,
                                std::vector<uint32>& out_polygon_path) const;
    /**
     * @brief XZ 平面で 2 つの線分の交点を求める。
     * @param path_start 一方の線分の始点。
     * @param path_end 一方の線分の終点。
     * @param edge_start もう一方の線分の始点。
     * @param edge_end もう一方の線分の終点。
     * @param out_path_t 交点の位置を path 側の線分上の比率で表した値。
     * @param out_edge_t 交点の位置を edge 側の線分上の比率で表した値。
     * @return 2 つの線分が交わる場合は true。
     */
    bool CalcSegmentIntersectionXZ(const Vec3& path_start, const Vec3& path_end,
                                   const Vec3& edge_start, const Vec3& edge_end,
                                   float& out_path_t, float& out_edge_t) const;
    /**
     * @brief XZ 平面で、点がポリゴンの内側にあるかを調べる。
     * @param mesh_data 対象の NavMesh。
     * @param polygon_index 対象のポリゴンインデックス。
     * @param point 調べる点。
     * @return 内側または辺上にある場合は true。
     */
    bool IsPointInsidePolygonXZ(const NavigationMeshData& mesh_data, uint32 polygon_index,
                                const Vec3& point) const;
    /**
     * @brief 指定した点を辺上に含むポリゴンを、隣接を辿って集める。
     * @param mesh_data 対象の NavMesh。
     * @param start_polygon_index 探索を始めるポリゴンインデックス。
     * @param point 集める基準になる点。
     * @param out_polygon_indices 集めたポリゴンのインデックス列。
     */
    void CollectConnectedPolygonsAtPoint(const NavigationMeshData& mesh_data, uint32 start_polygon_index,
                                         const Vec3& point, std::vector<uint32>& out_polygon_indices) const;
    /**
     * @brief XZ 平面で、点が線分上にあるかを調べる。
     * @param point 調べる点。
     * @param start 線分の始点。
     * @param end 線分の終点。
     * @return 線分上にある場合は true。
     */
    bool IsPointOnSegmentXZ(const Vec3& point, const Vec3& start, const Vec3& end) const;
    /**
     * @brief XZ 平面での符号付き面積の 2 倍を求める。
     * @param a 三角形の頂点。
     * @param b 三角形の頂点。
     * @param c 三角形の頂点。
     * @return 符号付き面積の 2 倍。反時計回りなら正になる。
     */
    float CalcSignedAreaXZ(const Vec3& a, const Vec3& b, const Vec3& c) const;
    /**
     * @brief XZ 平面で 2 点が同じ位置とみなせるかを調べる。
     * @param a 比較する点。
     * @param b 比較する点。
     * @return 同じ位置とみなせる場合は true。
     */
    bool IsSamePointXZ(const Vec3& a, const Vec3& b) const;
};

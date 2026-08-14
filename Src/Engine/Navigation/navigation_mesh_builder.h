#pragma once
#include "navigation_config.h"
#include <vector>

#include "navigation_geometry.h"
#include "../../Core/Math/intersect.h"
#include "navigation_cell_range.h"
#include "navigation_contour.h"
#include "navigation_span.h"
#include "navigation_type.h"

class NavigationHeightfield;
class NavigationCompactHeightfield;

/**
 * @brief 地形の三角形から NavMesh を組み立てる。
 *
 * 地形をボクセル化して歩ける床面を取り出し、床面を領域に分けて輪郭を抜き、
 * 輪郭を凸ポリゴンに分割して NavMesh にする、という順に処理を進める。
 * 高さを地形に沿わせる詳細メッシュも最後に作る。
 */
class NavigationMeshBuilder
{
public:
    /**
     * @brief 地形をボクセル化してハイトフィールドを作る。
     * @param geometries NavMesh の元になる地形の一覧。
     * @param config 構築パラメータ。
     * @param heightfield 結果を書き込むハイトフィールド。
     * @return 構築に成功した場合は true。
     */
    bool Build(const std::vector<NavigationGeometry>& geometries, const NavigationConfig& config,
               NavigationHeightfield& heightfield);
    /**
     * @brief 地形 1 つ分の三角形をハイトフィールドに書き込む。
     * @param geometry 書き込む地形。
     * @param config 構築パラメータ。
     * @param height 書き込み先のハイトフィールド。
     * @return 書き込みに成功した場合は true。
     */
    bool RasterizeGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                           NavigationHeightfield& height) const;
    /**
     * @brief 三角形 1 枚をハイトフィールドに書き込む。
     * @param tri 書き込む三角形。
     * @param config 構築パラメータ。
     * @param height 書き込み先のハイトフィールド。
     * @return 書き込みに成功した場合は true。
     */
    bool RasterizeTriangle(const Triangle& tri, const NavigationConfig& config, NavigationHeightfield& height) const;

    /**
     * @brief ハイトフィールドから歩ける床面だけを取り出す。
     * @param source 元になるハイトフィールド。
     * @param output 結果を書き込む形式。
     * @param config 構築パラメータ。
     * @return 構築に成功した場合は true。
     */
    bool BuildCompactHeightfield(NavigationHeightfield* source, NavigationCompactHeightfield& output,
                                 const NavigationConfig& config);
    /**
     * @brief 床面を、輪郭を抜ける単位の領域に分割する。
     * @param heightfield 対象のハイトフィールド。
     * @param config 構築パラメータ。
     * @return 分割に成功した場合は true。
     */
    bool BuildRegions(NavigationCompactHeightfield& heightfield, const NavigationConfig& config) const;
    /**
     * @brief 各領域の輪郭を抜き出し、単純化する。
     * @param heightfield 対象のハイトフィールド。
     * @param config 構築パラメータ。
     * @param contours 抜き出した輪郭の一覧。
     * @return 抽出に成功した場合は true。
     */
    bool BuildContours(NavigationCompactHeightfield& heightfield, const NavigationConfig& config,
                       std::vector<NavigationContour>& contours) const;
    /**
     * @brief 輪郭を凸ポリゴンに分割して NavMesh を作る。
     * @param heightfield セル座標からワールド座標への変換に使うハイトフィールド。
     * @param contours 元になる輪郭の一覧。
     * @param config 構築パラメータ。
     * @param mesh_data 結果を書き込む NavMesh。
     * @return 構築に成功した場合は true。
     */
    bool BuildNavigationMeshData(const NavigationCompactHeightfield& heightfield,
                                 const std::vector<NavigationContour>& contours,
                                 const NavigationConfig& config, NavigationMeshData& mesh_data) const;
    /**
     * @brief NavMesh の各ポリゴンを、地形の高さに沿う三角形に細分する。
     * @param heightfield 高さの参照元になるハイトフィールド。
     * @param mesh_data 元になる NavMesh。
     * @param config 構築パラメータ。
     * @param detail_mesh_data 結果を書き込む詳細メッシュ。
     * @return 構築に成功した場合は true。
     */
    bool BuildNavigationDetailMesh(const NavigationCompactHeightfield& heightfield, const NavigationMeshData& mesh_data,
                                   const NavigationConfig& config, NavigationDetailMeshData& detail_mesh_data) const;

private:
    /**
     * @brief 立体としてボクセル化するときの、垂直方向の交点。
     *
     * depth_delta は面の表裏を表し、これを足し合わせることで立体の内外を判定する。
     */
    struct SolidIntersection
    {
        float height = 0.0f;
        int32 depth_delta = 0;
        bool is_walkable_top = false;
    };
    /**
     * @brief 三角形を一定の細かさで分割し、詳細メッシュに追加する。
     * @param heightfield 高さの参照元になるハイトフィールド。
     * @param config 構築パラメータ。
     * @param a 三角形の頂点。
     * @param b 三角形の頂点。
     * @param c 三角形の頂点。
     * @param region_id 高さを参照する領域の ID。
     * @param detail_mesh_data 追加先の詳細メッシュ。
     */
    void AppendUniformDetailTriangle(
        const NavigationCompactHeightfield& heightfield,const NavigationConfig& config,
        const Vec3& a,const Vec3& b,const Vec3& c,uint32 region_id,NavigationDetailMeshData& detail_mesh_data) const;
    /**
     * @brief 三角形が地形からどれだけ離れているかの最大値を求める。
     * @param heightfield 高さの参照元になるハイトフィールド。
     * @param config 構築パラメータ。
     * @param a 三角形の頂点。
     * @param b 三角形の頂点。
     * @param c 三角形の頂点。
     * @param region_id 高さを参照する領域の ID。
     * @param out_position 最も離れていた位置。
     * @return 高さの差の最大値。
     */
    float CalcDetailTriangleMaxPenetration(
        const NavigationCompactHeightfield& heightfield, const NavigationConfig& config,
        const Vec3& a, const Vec3& b, const Vec3& c, uint32 region_id,
        Vec3& out_position) const;
    /**
     * @brief 指定セルの周辺から、基準の高さに最も近い床面の高さを探す。
     * @param heightfield 対象のハイトフィールド。
     * @param cell_x セルの X 座標。
     * @param cell_z セルの Z 座標。
     * @param region_id 対象の領域 ID。
     * @param reference_height 基準となる高さ。
     * @param out_height 見つかった高さ。
     * @return 見つかった場合は true。
     */
    bool TryGetClosestSpanFloorHeight(const NavigationCompactHeightfield& heightfield, int32 cell_x, int32 cell_z,
                                      uint32 region_id, float reference_height, float& out_height) const;
    /**
     * @brief 指定したワールド座標の地面の高さを求める。
     * @param heightfield 対象のハイトフィールド。
     * @param world_x サンプルする X 座標。
     * @param world_z サンプルする Z 座標。
     * @param region_id 対象の領域 ID。
     * @param reference_height 基準となる高さ。
     * @param out_height 求めた高さ。
     * @param max_height_diff 基準の高さから許容する差。
     * @return 高さが求まった場合は true。
     */
    bool TrySampleSurfaceHeight(const NavigationCompactHeightfield& heightfield,
                                float world_x, float world_z, uint32 region_id, float reference_height, float& out_height, float max_height_diff) const;
    /**
     * @brief 閉じた地形を、面ではなく中身の詰まった立体としてボクセル化する。
     * @param geometry 書き込む地形。
     * @param config 構築パラメータ。
     * @param heightfield 書き込み先のハイトフィールド。
     * @return 書き込みに成功した場合は true。
     */
    bool RasterizeSolidGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                                NavigationHeightfield& heightfield) const;
    /**
     * @brief 1 セル分の交点列から、立体の占有区間を求めて書き込む。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @param intersections そのセルで集めた交点列。
     * @param heightfield 書き込み先のハイトフィールド。
     * @return 書き込みに成功した場合は true。
     */
    bool RasterizeSolidCell(uint32 x, uint32 z, std::vector<SolidIntersection>& intersections,
                            NavigationHeightfield& heightfield) const;
    /**
     * @brief 各セルの中心から垂直に伸ばした線と地形の交点を集める。
     * @param geometry 対象の地形。
     * @param config 構築パラメータ。
     * @param heightfield セル配置の基準になるハイトフィールド。
     * @param out_cell_intersections セルごとの交点列。
     * @return 収集に成功した場合は true。
     */
    bool CollectSolidIntersections(const NavigationGeometry& geometry, const NavigationConfig& config,
                                   const NavigationHeightfield& heightfield,
                                   std::vector<std::vector<SolidIntersection>>& out_cell_intersections) const;
    /**
     * @brief 三角形と垂直な直線との交点の高さを求める。
     * @param triangle 対象の三角形。
     * @param sample_x 直線の X 座標。
     * @param sample_z 直線の Z 座標。
     * @param out_height 交点の高さ。
     * @return 交わる場合は true。
     */
    bool TryCalcVerticalIntersectionHeight(const Triangle& triangle, float sample_x, float sample_z,
                                           float& out_height) const;
    /**
     * @brief 地形が閉じた立体かどうかを調べる。
     * @param geometry 対象の地形。
     * @return 全ての辺が 2 枚の面で共有されている場合は true。
     */
    bool IsClosedGeometry(const NavigationGeometry& geometry) const;
    /**
     * @brief セルの角にあたる位置の地面の高さを求める。
     * @param heightfield 対象のハイトフィールド。
     * @param corner_x 角の X 座標。
     * @param corner_z 角の Z 座標。
     * @param region_id 対象の領域 ID。
     * @param ref_height 基準となる高さ。
     * @param max_height_diff 基準の高さから許容する差。
     * @return 求めた高さ。
     */
    float CalcSurfaceCornerHeight(const NavigationCompactHeightfield& heightfield, int32 corner_x, int32 corner_z,
                                  uint32 region_id, float ref_height, float max_height_diff) const;
    /**
     * @brief XZ 平面でのポリゴンの面積を求める。
     * @param vertices ポリゴンの頂点列。
     * @return 面積。
     */
    float CalcPolygonAreaXZ(const std::vector<Vec3>& vertices) const;
    /**
     * @brief 各ポリゴンの辺に、辺を共有する隣接ポリゴンを割り当てる。
     * @param mesh_data 対象の NavMesh。
     */
    void BuildPolygonAdjacency(NavigationMeshData& mesh_data) const;
    /**
     * @brief 輪郭の頂点に対応する NavMesh の頂点を返す。まだ無ければ追加する。
     * @param contour_vertex 元になる輪郭の頂点。
     * @param heightfield セル座標からワールド座標への変換に使うハイトフィールド。
     * @param mesh_data 対象の NavMesh。
     * @return 頂点のインデックス。
     */
    uint32 FindOrAddNavigationMeshVertex(const NavigationContourVertex& contour_vertex,
                                         const NavigationCompactHeightfield& heightfield,
                                         NavigationMeshData& mesh_data) const;
    /**
     * @brief 輪郭の頂点のセル座標をワールド座標に変換する。
     * @param vertex 変換する輪郭の頂点。
     * @param heightfield 変換の基準になるハイトフィールド。
     * @return ワールド座標。
     */
    Vec3 ConvertContourVertexToWorld(const NavigationContourVertex& vertex,
                                     const NavigationCompactHeightfield& heightfield) const;
    /**
     * @brief 輪郭を三角形分割し、隣り合う三角形をまとめて凸ポリゴンにする。
     * @param contour 元になる輪郭。
     * @param max_vertex_count 1 つのポリゴンが持てる頂点数の上限。
     * @param out_poly 作成したポリゴンの一覧。
     * @return 分割に成功した場合は true。
     */
    bool BuildContourPolygons(const NavigationContour& contour, uint32 max_vertex_count,
                              std::vector<NavigationContourPolygon>& out_poly) const;
    /**
     * @brief 2 つのポリゴンを、凸性と頂点数の条件を満たす場合に結合する。
     * @param contour 頂点の参照元になる輪郭。
     * @param fir 結合するポリゴン。
     * @param sec 結合するポリゴン。
     * @param max_vertex_count 結合後に許される頂点数の上限。
     * @param out_merged 結合したポリゴン。
     * @return 結合できた場合は true。
     */
    bool TryMergeContourPolygons(const NavigationContour& contour, const NavigationContourPolygon& fir,
                                 const NavigationContourPolygon& sec, uint32 max_vertex_count,
                                 NavigationContourPolygon& out_merged) const;
    /**
     * @brief ポリゴンが凸かどうかを調べる。
     * @param contour 頂点の参照元になる輪郭。
     * @param polygon 調べるポリゴン。
     * @return 凸である場合は true。
     */
    bool IsContourPolygonConvex(const NavigationContour& contour, const NavigationContourPolygon& polygon) const;
    /**
     * @brief 2 つのポリゴンが共有する辺を探す。
     * @param fir 一方のポリゴン。
     * @param sec もう一方のポリゴン。
     * @param out_fir_index fir 側の辺の位置。
     * @param out_sec_index sec 側の辺の位置。
     * @return 共有辺がある場合は true。
     */
    bool FindSharedPolygonEdge(const NavigationContourPolygon& fir, const NavigationContourPolygon& sec,
                               uint32& out_fir_index, uint32& out_sec_index) const;
    /**
     * @brief 耳刈り取り法で輪郭を三角形に分割する。
     * @param contour 分割する輪郭。
     * @param triangles 分割してできた三角形の一覧。
     * @return 分割に成功した場合は true。
     */
    bool TriangulateContour(const NavigationContour& contour, std::vector<NavigationContourTriangle>& triangles) const;
    /**
     * @brief 指定位置の頂点が、切り落とせる耳かどうかを調べる。
     * @param contour 対象の輪郭。
     * @param remaining_indices まだ切り落としていない頂点の並び。
     * @param remaining_position 調べる頂点の位置。
     * @return 耳である場合は true。
     */
    bool IsContourEar(const NavigationContour& contour, const std::vector<uint32>& remaining_indices,
                      uint32 remaining_position) const;
    /**
     * @brief XZ 平面で、点が三角形の内側または辺上にあるかを調べる。
     * @param point 調べる点。
     * @param a 三角形の頂点。
     * @param b 三角形の頂点。
     * @param c 三角形の頂点。
     * @return 内側または辺上にある場合は true。
     */
    bool IsPointInsideOrOnTriangleXZ(const NavigationContourVertex& point, const NavigationContourVertex& a,
                                     const NavigationContourVertex& b, const NavigationContourVertex& c) const;
    /**
     * @brief 穴を表す輪郭を、それを囲む外周の輪郭に橋渡しして 1 本にまとめる。
     * @param contours 対象の輪郭の一覧。穴は取り除かれる。
     * @return 統合に成功した場合は true。
     */
    bool MergeContourHoles(std::vector<NavigationContour>& contours) const;
    /**
     * @brief 穴を囲んでいる外周の輪郭を探す。
     * @param hole 穴を表す輪郭。
     * @param contours 探索対象の輪郭の一覧。
     * @param outer_index 見つかった外周の輪郭の位置。
     * @return 見つかった場合は true。
     */
    bool FindContainingOuterContour(const NavigationContour& hole, const std::vector<NavigationContour>& contours,
                                    uint32& outer_index) const;
    /**
     * @brief 穴の輪郭を、指定した頂点の組で外周の輪郭につなぐ。
     * @param outer つなぎ先の外周の輪郭。
     * @param hole つなぐ穴の輪郭。
     * @param outer_index 外周側のつなぐ頂点。
     * @param hole_index 穴側のつなぐ頂点。
     */
    void MergeHoleIntoContour(NavigationContour& outer, const NavigationContour& hole,
                              uint32 outer_index, uint32 hole_index) const;
    /**
     * @brief 穴と外周をつなげる、互いに見通せる頂点の組を探す。
     * @param outer 外周の輪郭。
     * @param hole_contour 穴の輪郭。
     * @param out_outer_index 外周側の頂点。
     * @param out_hole_index 穴側の頂点。
     * @return 見つかった場合は true。
     */
    bool FindHoleBridge(const NavigationContour& outer, const NavigationContour& hole_contour,
                        uint32& out_outer_index, uint32& out_hole_index) const;
    /**
     * @brief 2 頂点を結ぶ線が、輪郭の辺と交差せずに引けるかを調べる。
     * @param outer 外周の輪郭。
     * @param outer_index 外周側の頂点。
     * @param hole_contour 穴の輪郭。
     * @param hole_index 穴側の頂点。
     * @return 交差せずに引ける場合は true。
     */
    bool IsHoleBridgeVisible(const NavigationContour& outer, uint32 outer_index,
                             const NavigationContour& hole_contour, uint32 hole_index) const;
    /**
     * @brief XZ 平面で 2 つの線分が交差するかを調べる。
     * @param a 一方の線分の端点。
     * @param b 一方の線分の端点。
     * @param c もう一方の線分の端点。
     * @param d もう一方の線分の端点。
     * @return 交差する場合は true。
     */
    bool DoSegmentsIntersectXZ(const NavigationContourVertex& a, const NavigationContourVertex& b,
                               const NavigationContourVertex& c, const NavigationContourVertex& d) const;
    /**
     * @brief XZ 平面で、点が線分上にあるかを調べる。
     * @param point 調べる点。
     * @param start 線分の始点。
     * @param end 線分の終点。
     * @return 線分上にある場合は true。
     */
    bool IsPointOnSegmentXZ(const NavigationContourVertex& point, const NavigationContourVertex& start,
                            const NavigationContourVertex& end) const;
    /**
     * @brief XZ 平面での符号付き面積の 2 倍を、整数のまま求める。
     * @param a 三角形の頂点。
     * @param b 三角形の頂点。
     * @param c 三角形の頂点。
     * @return 符号付き面積の 2 倍。反時計回りなら正になる。
     */
    int64 CalcTriangleSignedAreaTwiceXZ(const NavigationContourVertex& a, const NavigationContourVertex& b,
                                        const NavigationContourVertex& c) const;
    /**
     * @brief 点が輪郭の内側にあるかを調べる。
     * @param point 調べる点。
     * @param contour 対象の輪郭。
     * @return 内側にある場合は true。
     */
    bool IsPointInsideContour(const NavigationContourVertex& point, const NavigationContour& contour) const;
    /**
     * @brief 輪郭の符号付き面積の 2 倍を求める。
     * @param contour 対象の輪郭。
     * @return 符号付き面積の 2 倍。反時計回りなら正、穴なら負になる。
     */
    int64 CalcContourSignedAreaTwice(const NavigationContour& contour) const;
    /**
     * @brief 輪郭から、形をほぼ保てる範囲で頂点を間引く。
     * @param raw_contour 元になる輪郭。
     * @param max_error_in_cells XZ 方向に許容するずれ。セル数で指定する。
     * @param max_height_error_in_cells 高さ方向に許容するずれ。セル数で指定する。
     * @param max_edge_len 1 本の辺の長さの上限。
     * @param simplified_contour 間引いた後の輪郭。
     * @return 単純化に成功した場合は true。
     */
    bool SimplifyContour(const NavigationContour& raw_contour, float max_error_in_cells,
                         float max_height_error_in_cells, float max_edge_len,
                         NavigationContour& simplified_contour) const;
    /**
     * @brief 点と線分の距離の 2 乗を求める。
     * @param point 対象の点。
     * @param start 線分の始点。
     * @param end 線分の終点。
     * @return 距離の 2 乗。
     */
    float CalcPointToSegmentDistanceSquared(const NavigationContourVertex& point, const NavigationContourVertex& start,
                                            const NavigationContourVertex& end) const;
    /**
     * @brief 点の高さが、線分を補間した高さからどれだけ離れているかを求める。
     * @param point 対象の点。
     * @param start 線分の始点。
     * @param end 線分の終点。
     * @return 高さの差。
     */
    float CalcPointToSegmentHeightError(const NavigationContourVertex& point,
                                        const NavigationContourVertex& start,
                                        const NavigationContourVertex& end) const;
    /**
     * @brief 輪郭の頂点にあたるセルの角の高さを求める。
     * @param heightfield 対象のハイトフィールド。
     * @param span_index 基準となるスパンの通し番号。
     * @param direction 角を決める方向。
     * @return 求めた高さ。
     */
    uint32 CalcContourCornerHeight(const NavigationCompactHeightfield& heightfield, uint32 span_index,
                                   uint32 direction) const;
    /**
     * @brief 領域の境界を辿って輪郭を 1 本抜き出す。
     * @param heightfield 対象のハイトフィールド。
     * @param start_x 辿り始めるセルの X 座標。
     * @param start_z 辿り始めるセルの Z 座標。
     * @param start_span_index 辿り始めるスパンの通し番号。
     * @param start_direction 辿り始める方向。
     * @param boundary_masks 境界の辺を記録したマスク。辿った辺は消す。
     * @param contour 抜き出した輪郭。
     * @return 抽出に成功した場合は true。
     */
    bool TraceRegionContour(const NavigationCompactHeightfield& heightfield, uint32 start_x, uint32 start_z,
                            uint32 start_span_index, uint32 start_direction, std::vector<uint8>& boundary_masks,
                            NavigationContour& contour) const;
    /**
     * @brief 各スパンの 4 方向について、そこが領域の境界かどうかを調べる。
     * @param heightfield 対象のハイトフィールド。
     * @return スパンごとの境界を表すマスク。
     */
    std::vector<uint8> BuildContourBoundaryMasks(NavigationCompactHeightfield& heightfield) const;
    /**
     * @brief 小さすぎる領域を、隣の領域に併合する。
     * @param heightfield 対象のハイトフィールド。
     * @param next_id 現在までに割り当てた領域 ID の次の値。
     * @param config 構築パラメータ。
     */
    void MergeSmallRegions(NavigationCompactHeightfield& heightfield, uint32 next_id,
                           const NavigationConfig& config) const;
    /**
     * @brief 併合できないほど小さく孤立した領域を取り除く。
     * @param heightfield 対象のハイトフィールド。
     * @param next_region_id 現在までに割り当てた領域 ID の次の値。
     * @param config 構築パラメータ。
     */
    void FilterSmallRegions(NavigationCompactHeightfield& heightfield, uint32 next_region_id,
                            const NavigationConfig& config) const;
    /**
     * @brief 既存の領域を、指定した壁からの距離まで広げる。
     * @param compact_heightfield 対象のハイトフィールド。
     * @param min_dist 広げる範囲の下限となる壁からの距離。
     */
    void ExpandRegionsAtLevel(NavigationCompactHeightfield& compact_heightfield, uint32 min_dist) const;
    /**
     * @brief どの領域にも属さない床面から、新しい領域を作る。
     * @param heightfield 対象のハイトフィールド。
     * @param min_dist 対象にする壁からの距離の下限。
     * @param next_region_id 割り当てる領域 ID。割り当てた分だけ進む。
     */
    void FloodNewRegionsAtLevel(NavigationCompactHeightfield& heightfield, uint32 min_dist,
                                uint32& next_region_id) const;
    /**
     * @brief エージェントの半径の分だけ、歩ける範囲を壁から削る。
     * @param heightfield 対象のハイトフィールド。
     * @param config 構築パラメータ。
     */
    void ErodeWalkableArea(NavigationCompactHeightfield& heightfield, const NavigationConfig& config);
    /**
     * @brief 隣り合う床面のうち、段差を越えられるものを接続する。
     * @param heightfield 対象のハイトフィールド。
     * @param config 構築パラメータ。
     * @return 接続に成功した場合は true。
     */
    bool BuildCompactConnections(NavigationCompactHeightfield& heightfield, const NavigationConfig& config);
    /**
     * @brief 崖のふちにあたるスパンを歩行不可にする。
     * @param heightfield 対象のハイトフィールド。
     * @param config 構築パラメータ。
     */
    void FilterLedgeSpans(NavigationHeightfield& heightfield, const NavigationConfig& config);
    /**
     * @brief 頭上が狭くて通れないスパンを歩行不可にする。
     * @param heightfield 対象のハイトフィールド。
     * @param config 構築パラメータ。
     */
    void FilterLowCeilingSpans(NavigationHeightfield& heightfield, const NavigationConfig& config);
    /**
     * @brief ワールド空間の高さの範囲から、セル単位のスパンを作る。
     * @param min_y 範囲の下端。
     * @param max_y 範囲の上端。
     * @param height セルの刻み幅の基準になるハイトフィールド。
     * @param is_walk 歩行可能として作る場合は true。
     * @param span 作成したスパン。
     * @return 作成に成功した場合は true。
     */
    bool CreateSpanFromHeightRange(float min_y, float max_y, const NavigationHeightfield& height,
                                   bool is_walk, NavigationSpan& span) const;

    /**
     * @brief 地形の指定位置の三角形を、ワールド空間に変換して取り出す。
     * @param geometry 対象の地形。
     * @param begin 三角形の先頭にあたるインデックスの位置。
     * @return ワールド空間の三角形。
     */
    Triangle GetWorldTriangle(const NavigationGeometry& geometry, uint32 begin) const;
    /**
     * @brief 三角形を包む軸平行な範囲を求める。
     * @param tri 対象の三角形。
     * @return 三角形を包む範囲。
     */
    Box CalcTriangleBounds(const Triangle& tri) const;
    /**
     * @brief 指定した範囲が重なるセルの走査範囲を求める。
     * @param b 対象の範囲。
     * @param height セル配置の基準になるハイトフィールド。
     * @param range 求めたセルの範囲。
     * @return 範囲がハイトフィールドと重なる場合は true。
     */
    bool CalcCellRange(const Box& b, const NavigationHeightfield& height, CellRange& range) const;
    /**
     * @brief 1 セルが占めるワールド空間の範囲を求める。
     * @param height 対象のハイトフィールド。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @return セルの範囲。
     */
    Box CalcCellBounds(const NavigationHeightfield& height, uint32 x, uint32 z) const;
    /**
     * @brief 三角形をセルの範囲で切り取る。
     * @param tri 切り取る三角形。
     * @param b 切り取りに使うセルの範囲。
     * @return 切り取ってできたポリゴンの頂点列。
     */
    std::vector<Vec3> ClipTriangleToCell(const Triangle& tri, const Box& b) const;
    /**
     * @brief ポリゴンの高さの範囲を求める。
     * @param vertices 対象のポリゴンの頂点列。
     * @param low_height 範囲の下端。
     * @param high_height 範囲の上端。
     */
    void CalcPolygonHeightRange(const std::vector<Vec3>& vertices, float& low_height, float& high_height) const;
};

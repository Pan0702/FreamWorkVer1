#pragma once
#include <vector>

#include "navigation_heightfield_cell.h"
#include "../../Core/Math/intersect.h"


/**
 * @brief 地形をボクセル化して保持するハイトフィールド。
 *
 * XZ 平面をセルに区切り、各セルが高さ方向のスパン列を持つ。
 * NavMesh 構築の最初の段階として、地形の三角形をここへ書き込む。
 */
class NavigationHeightfield
{
public:
    /**
     * @brief 覆う範囲とセルの大きさを決めて、空のセルを用意する。
     * @param aabb ボクセル化するワールド空間の範囲。
     * @param size XZ 方向のセルの一辺の長さ。
     * @param height 高さ方向のセルの刻み幅。
     * @return 初期化に成功した場合は true。
     */
    bool Initialize(const Box& aabb, float size, float height);
    /**
     * @brief 指定した格子座標のセルを取得する。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @return 該当するセル。範囲外の場合は nullptr。
     */
    NavigationHeightfieldCell* GetCell(uint32 x, uint32 z);
    /**
     * @brief 指定したセルにスパンを追加する。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @param span 追加するスパン。
     * @return 追加に成功した場合は true。
     */
    bool AddSpan(uint32 x, uint32 z, const NavigationSpan& span);
    /**
     * @brief 各セル内で重なり合うスパンを 1 つにまとめる。
     */
    void MergeSpans();
    /**
     * @brief ボクセル化した範囲を取得する。
     * @return ワールド空間での範囲。
     */
    const Box& GetWorldBounds() const;
    /**
     * @brief XZ 方向のセルの一辺の長さを取得する。
     * @return セルの一辺の長さ。
     */
    float GetCellSize() const;
    /**
     * @brief 高さ方向のセルの刻み幅を取得する。
     * @return 高さの刻み幅。
     */
    float GetCellHeight() const;
    /**
     * @brief X 方向のセル数を取得する。
     * @return セル数。
     */
    uint32 GetWidth() const;
    /**
     * @brief Z 方向のセル数を取得する。
     * @return セル数。
     */
    uint32 GetDepth() const;
private:
    Box world_bounds_ = {};
    float cell_size_ = 0.0f;
    float cell_height_= 0.0f;
    uint32 width_ = 0;
    uint32 depth_ = 0;
    std::vector<NavigationHeightfieldCell> cells_;
};

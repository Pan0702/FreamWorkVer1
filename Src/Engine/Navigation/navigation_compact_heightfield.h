#pragma once
#include <vector>
#include "navigation_span.h"
#include "../../Core/Math/intersect.h"

class NavigationHeightfield;

/**
 * @brief 歩行可能な床面だけを抜き出した、隣接情報付きのハイトフィールド。
 *
 * NavigationHeightfield が立体の占有区間を持つのに対し、こちらは床面だけを
 * 1 本の配列にまとめて持ち、各セルはその範囲を指す。領域分割や輪郭抽出は
 * この形式の上で行う。
 */
class NavigationCompactHeightfield
{
public:
    /**
     * @brief ボクセル化済みのハイトフィールドから、歩行可能な床面を取り出す。
     * @param source_heightfield 元になるハイトフィールド。
     * @return 初期化に成功した場合は true。
     */
    bool Initialize(const NavigationHeightfield& source_heightfield);
    /**
     * @brief 指定したセルに床面スパンを追加する。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @param span 追加するスパン。
     * @return 追加に成功した場合は true。
     */
    bool AddSpan(uint32 x, uint32 z, const NavigationCompactSpan& span);
    /**
     * @brief 指定した格子座標のセルを取得する。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @return 該当するセル。範囲外の場合は nullptr。
     */
    NavigationCompactCell* GetCell(uint32 x, uint32 z);
    /**
     * @brief 指定した格子座標のセルを取得する。
     * @param x セルの X 座標。
     * @param z セルの Z 座標。
     * @return 該当するセル。範囲外の場合は nullptr。
     */
    const NavigationCompactCell* GetCell(uint32 x, uint32 z) const;
    /**
     * @brief 通し番号でスパンを取得する。
     * @param span_index スパンの通し番号。
     * @return 該当するスパン。範囲外の場合は nullptr。
     */
    NavigationCompactSpan* GetSpan(uint32 span_index);
    /**
     * @brief 通し番号でスパンを取得する。
     * @param span_index スパンの通し番号。
     * @return 該当するスパン。範囲外の場合は nullptr。
     */
    const NavigationCompactSpan* GetSpan(uint32 span_index) const;
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
    /**
     * @brief XZ 方向のセルの一辺の長さを取得する。
     * @return セルの一辺の長さ。
     */
    float GetCellSize() const;
    /**
     * @brief ボクセル化した範囲を取得する。
     * @return ワールド空間での範囲。
     */
    const Box& GetWorldBounds() const;
    /**
     * @brief 全ての床面スパンを取得する。
     * @return スパンの配列。セルはこの配列への範囲として床面を指す。
     */
    const std::vector<NavigationCompactSpan>& GetSpans() const;
private:
    std::vector<NavigationCompactCell> cells_;
    std::vector<NavigationCompactSpan> spans_;
    Box world_bounds_;
    float cell_size_;
    float cell_height_;
    uint32 width_;
    uint32 depth_;
};



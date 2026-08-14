#pragma once
#include "../Engine/Navigation/navigation_type.h"

/**
 * @brief NavMesh の形状や経路をデバッグ表示する。
 *
 * 状態を持たないため使い捨てで構わない。
 */
class NavigationDebugRenderer
{
public:
    /**
     * @brief NavMesh のポリゴンと辺を描画する。
     *
     * ポリゴンは領域ごとに色を変えて塗り、辺は隣接があれば緑、
     * 隣接がない境界であれば赤で描く。
     * @param mesh_data 描画する NavMesh。
     * @param height_offset 地形と重なってちらつくのを防ぐために持ち上げる高さ。
     */
    void Draw(const NavigationMeshData& mesh_data, float height_offset = 0.2f) const;
    /**
     * @brief 経路探索が通ったポリゴン列を描画する。
     *
     * 塗りつぶしの色は経路の順に緑から赤へ変化する。
     * @param mesh_data 対象の NavMesh。
     * @param polygon_indices 描画するポリゴンのインデックス列。
     * @param height_offset 地形と重なってちらつくのを防ぐために持ち上げる高さ。
     */
    void DrawCorridor(const NavigationMeshData& mesh_data,
                      const std::vector<uint32_t>& polygon_indices,
                      float height_offset = 1.0f) const;
    /**
     * @brief 高さを地形に沿わせた詳細メッシュを描画する。
     * @param detail_mesh_data 描画する詳細メッシュ。
     * @param height_offset 地形と重なってちらつくのを防ぐために持ち上げる高さ。
     */
    void Draw(const NavigationDetailMeshData& detail_mesh_data, float height_offset = 0.2f) const;
};

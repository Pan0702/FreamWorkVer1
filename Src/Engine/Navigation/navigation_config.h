#pragma once
#include <cstdint>

/**
 * @brief NavMesh生成のパラメータ。
 *
 * 全ての長さはワールド単位で指定する。頂点データと同じ物差しでなければならない。
 * このプロジェクトのワールド単位は、キャラクター(remy)の全高が約3.8になる大きさ。
 * cm基準の値(agent_height=180など)を混ぜると、エージェントがステージより巨大に
 * なり、床が天井フィルタで消えてNavMeshが崩壊するので注意。
 */
struct NavigationConfig
{
    // ボクセルの解像度。cell_sizeはagent_radiusの1/2～1/3程度が目安。
    float cell_size = 0.3f;
    float cell_height = 0.2f;
    float max_edge_len = 12.0f;

    // エージェントの寸法。
    float agent_height = 3.8f;
    float agent_radius = 0.4f;
    float agent_max_climb = 0.6f;
    float agent_max_slope_deg = 45.0f;

    // 輪郭の単純化許容誤差。ワールド単位で指定する。
    float max_contour_simplification_error = 0.39f;
    // XZ上では直線でも、坂の始点・終点など高さが折れる頂点を残すための許容誤差。
    float max_contour_height_error = 0.1f;

    // 小さすぎるRegionの除去・統合のしきい値(Span数)。
    uint32_t min_region_span_count = 8;
    uint32_t merge_region_span_count = 200;

    uint32_t max_vertex_per_poly = 6;
    float detail_sample_max_error = 0.1f;
    uint32_t detail_max_subdivision_depth = 4;
    uint32_t detail_subdivision_count = 2;
    float detail_sample_distance = 1.2f;
};

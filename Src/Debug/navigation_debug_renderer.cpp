#include "navigation_debug_renderer.h"
#include "../Core/Math/my_math.h"
#include "../Debug/debug.h"

namespace
{
    // 面と線が同じ高さにあるとZファイティングでちらつくため、辺だけ少し持ち上げる。
    constexpr float kEdgeLift = 0.5f;
    // コリドーの辺を、塗りつぶしより上に描くための持ち上げ量。
    constexpr float kCorridorEdgeLift = 0.2f;
}

void NavigationDebugRenderer::Draw(const NavigationMeshData& mesh_data, float height_offset) const
{
    // 隣り合う領域が同じ色にならない程度に用意した色。region_idを巡回させて使う。
    const Vec4 region_colors[] =
    {
        Vec4(0.20f, 0.80f, 0.30f, 0.28f),
        Vec4(0.20f, 0.50f, 1.00f, 0.28f),
        Vec4(1.00f, 0.55f, 0.15f, 0.28f),
        Vec4(0.75f, 0.30f, 1.00f, 0.28f),
        Vec4(0.10f, 0.85f, 0.85f, 0.28f),
        Vec4(1.00f, 0.25f, 0.30f, 0.28f),
    };

    constexpr uint32 kRegionColorCount = 6;

    // 赤は隣接ポリゴンがない辺。歩ける範囲の内側に赤が出ていれば、
    // 本来つながるべき場所が分断されているというNavMeshの不具合を示す。
    const Vec4 boundary_color =
        Vec4(1.0f, 0.0f, 0.0f, 1.0f);

    const Vec4 connected_edge_color =
        Vec4(0.0f, 1.0f, 0.0f, 1.0f);
    for (uint32 polygon_index = 0;
         polygon_index < mesh_data.polygons.size();
         ++polygon_index)
    {
        const NavigationMeshPolygon& polygon =
            mesh_data.polygons[polygon_index];

        const uint32 vertex_count =
            static_cast<uint32>(
                polygon.vertex_indices.size());

        if (vertex_count < 3)
        {
            continue;
        }

        bool valid_polygon = true;

        for (uint32 vertex_index : polygon.vertex_indices)
        {
            if (vertex_index >= mesh_data.vertices.size())
            {
                valid_polygon = false;
                break;
            }
        }

        if (!valid_polygon)
        {
            continue;
        }

        const Vec4& fill_color = region_colors[polygon.region_id % kRegionColorCount];
        Vec3 first = mesh_data.vertices[polygon.vertex_indices[0]];
        first.y += height_offset;

        // 凸ポリゴンなので、先頭の頂点から三角形ファンとして塗れる。
        for (uint32 i = 1; i + 1 < vertex_count; ++i)
        {
            Vec3 second = mesh_data.vertices[polygon.vertex_indices[i]];

            Vec3 third = mesh_data.vertices[polygon.vertex_indices[i + 1]];

            second.y += height_offset;
            third.y += height_offset;

            Debug::Get().DrawTriangle3D(first, second, third, fill_color);
        }
        const bool has_neighbor_data = polygon.neighbor_polygon_indices.size() == vertex_count;
        for (uint32 edge = 0; edge < vertex_count; ++edge)
        {
            const uint32 next = (edge + 1) % vertex_count;

            const uint32 neighbor_index = has_neighbor_data
                                              ? polygon.neighbor_polygon_indices[edge]
                                              : UINT32_MAX;

            const bool is_boundary = neighbor_index == UINT32_MAX;

            // 内部辺は両ポリゴンから登録されるため片側だけ描く
            if (!is_boundary && polygon_index > neighbor_index)
            {
                continue;
            }

            Vec3 start = mesh_data.vertices[polygon.vertex_indices[edge]];

            Vec3 end = mesh_data.vertices[polygon.vertex_indices[next]];

            start.y += height_offset + kEdgeLift;
            end.y += height_offset + kEdgeLift;

            Debug::Get().DrawLine3D(start, end, is_boundary
                                                    ? boundary_color
                                                    : connected_edge_color);
        }
    }
}

void NavigationDebugRenderer::DrawCorridor(const NavigationMeshData& mesh_data,
                                            const std::vector<uint32>& polygon_indices,
                                            float height_offset) const
{
    if (polygon_indices.empty())
    {
        return;
    }

    const Vec4 edge_color(1.0f, 1.0f, 0.0f, 1.0f);
    // 経路の進み具合を0～1に正規化するための分母。
    // ポリゴンが1枚しかない場合の0除算を避けている。
    const float denominator = polygon_indices.size() > 1
                                  ? static_cast<float>(polygon_indices.size() - 1)
                                  : 1.0f;

    for (uint32 path_index = 0; path_index < polygon_indices.size(); ++path_index)
    {
        const uint32 polygon_index = polygon_indices[path_index];
        if (polygon_index >= mesh_data.polygons.size())
        {
            continue;
        }

        const NavigationMeshPolygon& polygon = mesh_data.polygons[polygon_index];
        const uint32 vertex_count = static_cast<uint32>(polygon.vertex_indices.size());
        if (vertex_count < 3)
        {
            continue;
        }

        bool valid_polygon = true;
        for (const uint32 vertex_index : polygon.vertex_indices)
        {
            if (vertex_index >= mesh_data.vertices.size())
            {
                valid_polygon = false;
                break;
            }
        }
        if (!valid_polygon)
        {
            continue;
        }

        // 始点側を緑、終点側を赤にして、経路をどちら向きに進むのか分かるようにする。
        const float path_ratio = static_cast<float>(path_index) / denominator;
        const Vec4 fill_color(
            0.15f + 0.85f * path_ratio,
            1.0f - 0.75f * path_ratio,
            0.05f,
            0.65f);

        Vec3 first = mesh_data.vertices[polygon.vertex_indices[0]];
        first.y += height_offset;
        for (uint32 vertex_index = 1; vertex_index + 1 < vertex_count; ++vertex_index)
        {
            Vec3 second = mesh_data.vertices[polygon.vertex_indices[vertex_index]];
            Vec3 third = mesh_data.vertices[polygon.vertex_indices[vertex_index + 1]];
            second.y += height_offset;
            third.y += height_offset;
            Debug::Get().DrawTriangle3D(first, second, third, fill_color);
        }

        // コリドーは通過するポリゴンを見せるのが目的なので、辺は隣接に関係なく全て描く。
        for (uint32 edge_index = 0; edge_index < vertex_count; ++edge_index)
        {
            Vec3 edge_start = mesh_data.vertices[polygon.vertex_indices[edge_index]];
            Vec3 edge_end = mesh_data.vertices[
                polygon.vertex_indices[(edge_index + 1) % vertex_count]];
            edge_start.y += height_offset + kCorridorEdgeLift;
            edge_end.y += height_offset + kCorridorEdgeLift;
            Debug::Get().DrawLine3D(edge_start, edge_end, edge_color);
        }
    }
}

void NavigationDebugRenderer::Draw(const NavigationDetailMeshData& detail_mesh_data, float height_offset) const
{
    const Vec4 fill_color = Vec4(0.20f, 0.50f, 1.00f, 0.28f);

    const Vec4 edge_color = Vec4(0.05f, 0.10f, 0.15f, 1.00f);

    // 詳細メッシュはインデックス3つで1三角形。
    for (uint32 i = 0; i + 2 < detail_mesh_data.indices.size(); i += 3)
    {
        const uint32 index_a = detail_mesh_data.indices[i];
        const uint32 index_b = detail_mesh_data.indices[i + 1];
        const uint32 index_c = detail_mesh_data.indices[i + 2];

        if (index_a >= detail_mesh_data.vertices.size() ||
            index_b >= detail_mesh_data.vertices.size() ||
            index_c >= detail_mesh_data.vertices.size())
        {
            continue;
        }

        Vec3 a = detail_mesh_data.vertices[index_a];
        Vec3 b = detail_mesh_data.vertices[index_b];
        Vec3 c = detail_mesh_data.vertices[index_c];

        a.y += height_offset;
        b.y += height_offset;
        c.y += height_offset;

        Debug::Get().DrawTriangle3D(a, b, c, fill_color);

        // 三角形が細かく数が多いため、辺まで描くと画面が埋まる。必要なときだけ有効にする。
        // Debug::Get().DrawLine3D(a, b, edge_color);
        // Debug::Get().DrawLine3D(b, c, edge_color);
        // Debug::Get().DrawLine3D(c, a, edge_color);
    }
}

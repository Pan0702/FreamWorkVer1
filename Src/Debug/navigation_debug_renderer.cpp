#include "navigation_debug_renderer.h"
#include "../Core/Math/my_math.h"
#include "../Debug/debug.h"

void NavigationDebugRenderer::Draw(const NavigationMeshData& mesh_data, float height_offset) const
{
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

    const Vec4 boundary_color =
        Vec4(1.0f, 0.9f, 0.1f, 1.0f);

    const Vec4 internal_edge_color =
        Vec4(0.05f, 0.1f, 0.15f, 1.0f);
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

        for (uint32 i = 1; i + 1 < vertex_count; ++i)
        {
            Vec3 second = mesh_data.vertices[polygon.vertex_indices[i]];

            Vec3 third = mesh_data.vertices[polygon.vertex_indices[i + 1]];

            second.y += height_offset;
            third.y += height_offset;

            Debug::Get().DrawTriangle3D(first, second, third, fill_color);
        }
        // 面の三角形分割線を描画
        for (uint32 i = 2; i + 1 < vertex_count; ++i)
        {
            Vec3 start =
                mesh_data.vertices[polygon.vertex_indices[0]];

            Vec3 end =
                mesh_data.vertices[polygon.vertex_indices[i]];

            // 面との重なりによるちらつきを防ぐ
            start.y += height_offset + 0.5f;
            end.y += height_offset + 0.5f;

            Debug::Get().DrawLine3D(start, end, internal_edge_color);
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

            start.y += height_offset + 0.5f;
            end.y += height_offset + 0.5f;

            Debug::Get().DrawLine3D(start, end, is_boundary
                                                    ? boundary_color
                                                    : internal_edge_color);
        }
    }
}

void NavigationDebugRenderer::Draw(const NavigationDetailMeshData& detail_mesh_data, float height_offset) const
{
    const Vec4 fill_color = Vec4(0.20f, 0.50f, 1.00f, 0.28f);

    const Vec4 edge_color = Vec4(0.05f, 0.10f, 0.15f, 1.00f);

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

        // Debug::Get().DrawLine3D(a, b, edge_color);
        // Debug::Get().DrawLine3D(b, c, edge_color);
        // Debug::Get().DrawLine3D(c, a, edge_color);
    }
}

#include "navigation_system.h"
#include <algorithm>

#include "navigation_compact_heightfield.h"
#include "navigation_heightfield.h"
#include "navigation_mesh_builder.h"
#include "navigation_mesh_query.h"
#include "../Components/navigation_source_component.h"
#include "../../Debug/debug.h"
#include "../../Debug/navigation_debug_renderer.h"

uint32 NavigationSystem::RegisterSource(NavigationSourceComponent* component)
{
    if (component == nullptr)
    {
        return 0;
    }
    // 二重登録された場合は、最初に割り当てたIDをそのまま返す。
    for (const auto& s : sources_)
    {
        if (s.component == component)
        {
            return s.id;
        }
    }

    RegisteredSource r;
    r.id = next_id_;
    r.component = component;
    sources_.push_back(r);
    next_id_++;
    return r.id;
}

void NavigationSystem::UnregisterSource(uint32 source_id)
{
    // 0は「登録されていない」ことを表すIDなので、割り当てにも解除にも使わない。
    if (source_id == 0)
    {
        return;
    }
    auto it = std::ranges::find(sources_, source_id, &RegisteredSource::id);
    if (it != sources_.end())
    {
        sources_.erase(it);
    }
}

std::vector<NavigationGeometry> NavigationSystem::CollectGeometries() const
{
    std::vector<NavigationGeometry> geometries;
    for (const auto& s : sources_)
    {
        if (s.component == nullptr)
        {
            continue;
        }
        const NavigationGeometry geo = s.component->GetGeometry();
        // 三角形として解釈できない形状は、以降の処理が破綻するので捨てる。
        if (geo.indices.size() % 3 == 0 && !geo.indices.empty() && !geo.vertices.empty())
        {
            geometries.push_back(geo);
        }
    }
    return geometries;
}

bool NavigationSystem::Rebuild(const NavigationConfig& config)
{
    const std::vector<NavigationGeometry> geometries =
        CollectGeometries();

    DEBUG_LOG(
        "[Navigation] rebuild_begin: sources=%zu, geometries=%zu",
        sources_.size(),
        geometries.size());

    if (geometries.empty())
    {
        DEBUG_LOG("[Navigation] rebuild_failed: no geometry");
        return false;
    }

    // 全ての地形を包む範囲を求め、そこをボクセル化の対象にする。
    Box world_bounds = geometries[0].world_bounds;

    for (uint32 i = 1; i < geometries.size(); ++i)
    {
        const Box& bounds = geometries[i].world_bounds;

        world_bounds.min.x =
            (std::min)(world_bounds.min.x, bounds.min.x);
        world_bounds.min.y =
            (std::min)(world_bounds.min.y, bounds.min.y);
        world_bounds.min.z =
            (std::min)(world_bounds.min.z, bounds.min.z);

        world_bounds.max.x =
            (std::max)(world_bounds.max.x, bounds.max.x);
        world_bounds.max.y =
            (std::max)(world_bounds.max.y, bounds.max.y);
        world_bounds.max.z =
            (std::max)(world_bounds.max.z, bounds.max.z);
    }
    // 一番低い床がセルの境界と重なって取りこぼされないよう、1セル分だけ下へ広げる。
    world_bounds.min.y -= config.cell_height;

    // 上端は、一番高い床の上にエージェントが立てる高さが必要になる。
    // ここが足りないと、最上段の床が天井フィルタで歩行不可にされてしまう。
    world_bounds.max.y += config.agent_height + config.agent_max_climb + config.cell_height;
    NavigationHeightfield heightfield;

    if (!heightfield.Initialize(world_bounds, config.cell_size, config.cell_height))
    {
        DEBUG_LOG("[Navigation] rebuild_failed: heightfield initialization");
        return false;
    }

    // 以降はNavMeshの構築手順。前の段の結果を次の段が受け取るため、順番は入れ替えられない。
    // ボクセル化 → 床面の抽出 → 領域分割と輪郭抽出 → 凸ポリゴン化 → 高さの詳細化。
    NavigationMeshBuilder builder;

    if (!builder.Build(geometries, config, heightfield))
    {
        DEBUG_LOG("[Navigation] rebuild_failed: heightfield rasterization");
        return false;
    }

    NavigationCompactHeightfield compact_heightfield;

    if (!builder.BuildCompactHeightfield(&heightfield, compact_heightfield, config))
    {
        DEBUG_LOG("[Navigation] rebuild_failed: compact heightfield");
        return false;
    }

    std::vector<NavigationContour> contours;

    if (!builder.BuildContours(compact_heightfield, config, contours))
    {
        DEBUG_LOG("[Navigation] rebuild_failed: contours");
        return false;
    }

    NavigationMeshData generated_mesh_data;

    if (!builder.BuildNavigationMeshData(compact_heightfield, contours, config, generated_mesh_data))
    {
        DEBUG_LOG("[Navigation] rebuild_failed: polygon mesh");
        return false;
    }

    NavigationDetailMeshData generated_detail_mesh_data;

    if (!builder.BuildNavigationDetailMesh(
            compact_heightfield,
            generated_mesh_data,
            config,
            generated_detail_mesh_data))
    {
        DEBUG_LOG("[Navigation] rebuild_failed: detail mesh");
        return false;
    }

    // 全ての段が成功してから差し替える。途中で失敗した場合は古いNavMeshが残る。
    mesh_data_.vertices.swap(generated_mesh_data.vertices);
    mesh_data_.polygons.swap(generated_mesh_data.polygons);

    detail_mesh_data_.vertices.swap(generated_detail_mesh_data.vertices);
    detail_mesh_data_.indices.swap(generated_detail_mesh_data.indices);
    return true;
}

bool NavigationSystem::FindPath(const Vec3& start_position, const Vec3& goal_position,
                                std::vector<Vec3>& out_path,
                                std::vector<uint32>* out_polygon_path) const
{
    out_path.clear();
    if (out_polygon_path != nullptr)
    {
        out_polygon_path->clear();
    }

    if (mesh_data_.vertices.empty() || mesh_data_.polygons.empty())
    {
        return false;
    }

    // クエリは状態を持たないので、毎回作り捨てて構わない。
    NavigationMeshQuery query;
    return query.FindPath(mesh_data_, start_position, goal_position,
                          out_path, out_polygon_path);
}

const NavigationDetailMeshData& NavigationSystem::GetDetailMeshData() const
{
    return detail_mesh_data_;
}

const NavigationMeshData& NavigationSystem::GetMeshData() const
{
    return mesh_data_;
}

void NavigationSystem::DrawDebug() const
{
    if (!debug_draw_enabled_)
    {
        return;
    }

    if (mesh_data_.vertices.empty() ||
        mesh_data_.polygons.empty())
    {
        return;
    }

    if (detail_mesh_data_.vertices.empty() ||
    detail_mesh_data_.indices.empty())
    {
        return;
    }

    NavigationDebugRenderer renderer;
    renderer.Draw(mesh_data_);
}

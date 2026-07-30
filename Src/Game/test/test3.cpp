#include "test3.h"

#include "../../Engine/world.h"
#include "objects/plane.h"
#include "../../Play/Player/player_camera.h"
#include "../../Play/Player/player.h"
#include "../../Engine/Navigation/navigation_mesh_query.h"

Test3::Test3()
{
}

Test3::~Test3()
{
}

void Test3::OnEnter()
{
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Plane>();
    NavigationConfig config{};
    config.min_region_span_count = 1;
    config.agent_radius = 0.0f;
    config.cell_height = 1.0f;
    config.cell_size = 1.0f;
    const bool navmesh_built = world_->GetNavigationSystem().Rebuild(config);
    if (navmesh_built)
    {
        const Vec3 start_position(-40.0f, 1.0f, -40.0f);
        const Vec3 goal_position(-40.0f, 1.0f, 40.0f);

        NavigationMeshQuery query;

        const bool path_found = query.FindPath(
            world_->GetNavigationSystem().GetMeshData(),
            start_position,
            goal_position,
            navigation_path_);
        DEBUG_LOG(
            "[Navigation] path_found=%s, point_count=%zu",
            path_found ? "true" : "false",
            navigation_path_.size());
    }
    LevelBase::OnEnter();
}

void Test3::Tick(float dt)
{
    const Vec3 draw_offset(0.0f, 2.0f, 0.0f);

    for (uint32 i = 0;
         i + 1 < navigation_path_.size();
         ++i)
    {
        const Vec3 start =
            navigation_path_[i] + draw_offset;

        const Vec3 end =
            navigation_path_[i + 1] + draw_offset;

        Debug::Get().DrawLine3D(
            start,
            end,
            Vec4(1.0f, 0.0f, 1.0f, 1.0f));
    }

    if (!navigation_path_.empty())
    {
        Debug::Get().DrawSphere3D(
            navigation_path_.front() + draw_offset,
            3.0f,
            Vec4(0.0f, 1.0f, 0.0f, 1.0f));

        Debug::Get().DrawSphere3D(
            navigation_path_.back() + draw_offset,
            3.0f,
            Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    LevelBase::Tick(dt);
}

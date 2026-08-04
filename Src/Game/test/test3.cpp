#include "test3.h"

#include "../../Engine/world.h"
#include "objects/plane.h"
#include "../../Play/Player/player_camera.h"
#include "../../Play/Player/player.h"
#include "../../Engine/Navigation/navigation_mesh_query.h"
#include "../../Engine/Components/navigation_agent_component.h"
#include "objects/slope.h"

Test3::Test3()
{
}

Test3::~Test3()
{
}

void Test3::OnEnter()
{
    player = SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    //SpawnActor<Plane>(Vec3(0.0f, 0.0f, 0.0f));
    // SpawnActor<Plane>(Vec3(100.0f, 0.0f, 0.0f));
    // SpawnActor<Plane>(Vec3(100.0f, 0.0f, 100.0f));
    SpawnActor<Slope>();
    // 既定値がこのプロジェクトのワールドスケールに合わせてあるのでそのまま使う。
    // 一部だけ上書きすると単位が混ざってNavMeshが崩れるため、変えるときは
    // navigation_config.h のコメントにある前提を確認すること。
    NavigationConfig config{};
    const bool navmesh_built = world_->GetNavigationSystem().Rebuild(config);
    navigation_agent = player->AddComponent<NavigationAgentComponent>();

    navigation_agent->SetDebugDrawEnabled(true);

    const bool path_found =
        navigation_agent->SetDestination(
            Vec3(50.0f, 0.0f, 80.0f));
    
    LevelBase::OnEnter();
}

void Test3::Tick(float dt)
{
    const bool path_found =
        navigation_agent->SetDestination(
            Vec3(50.0f, 0.0f, 80.0f));
    LevelBase::Tick(dt);
}

#include "test3.h"

#include "../../Engine/world.h"
#include "objects/plane.h"
#include "../../Play/Player/player_camera.h"
#include "../../Play/Player/player.h"
#include "../../Engine/Navigation/navigation_mesh_query.h"
#include "../../Engine/ai_controller.h"
#include "enemy/enemy_01.h"
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
    NavigationConfig config{};
    const bool navmesh_built = world_->GetNavigationSystem().Rebuild(config);

    // NavMesh構築後に敵とAIを配線する(MoveToActor内でNavigationAgentを付与するため)
    auto* enemy = SpawnActor<Enemy01>();
    auto* ai = SpawnActor<AiController>();
    ai->Possess(enemy);
    ai->MoveToActor(player);

    LevelBase::OnEnter();
}

void Test3::Tick(float dt)
{
    LevelBase::Tick(dt);
}

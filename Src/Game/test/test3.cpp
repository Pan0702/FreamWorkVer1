#include "test3.h"

#include "objects/instance_trees.h"
#include "objects/plane.h"
#include "../../Play/Player/player_camera.h"
#include "../../Play/Player/player.h"
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

    LevelBase::OnEnter();
}

void Test3::Tick(float dt)
{
    LevelBase::Tick(dt);
}

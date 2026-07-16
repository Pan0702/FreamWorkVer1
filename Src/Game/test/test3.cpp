#include "test3.h"

#include "objects/instance_trees.h"
#include "objects/plane.h"

Test3::Test3()
{
}

Test3::~Test3()
{
}

void Test3::OnEnter()
{
    SpawnActor<Plane>();
    SpawnActor<InstanceTrees>();
    LevelBase::OnEnter();
}

void Test3::Tick(float dt)
{
    LevelBase::Tick(dt);
}

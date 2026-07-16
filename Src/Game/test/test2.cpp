#include "test2.h"

#include "objects/constant.h"
#include "objects/plane.h"
#include "objects/tree.h"
#include "../../Play/Player/player_camera.h"
#include "../../Play/Player/player.h"

Test2::Test2()
{
    
}

Test2::~Test2() = default;

void Test2::OnEnter()
{
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Plane>();
    for (int w = kRange; w > -kRange; --w)
    {
        for (int h = kRange; h > -kRange; --h)
        {
            SpawnActor<Tree>(Vec3(w, 0.0f, h));
        }
    }
    LevelBase::OnEnter();
}

void Test2::Tick(float dt)
{
    LevelBase::Tick(dt);
}

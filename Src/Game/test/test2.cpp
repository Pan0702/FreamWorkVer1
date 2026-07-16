#include "test2.h"

#include "objects/plane.h"
#include "objects/tree.h"

Test2::Test2()
{
    
}

Test2::~Test2() = default;

void Test2::OnEnter()
{
    SpawnActor<Plane>();
    for (int w = 30; w > -30; --w)
    {
        for (int h = 30; h > -30; --h)
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

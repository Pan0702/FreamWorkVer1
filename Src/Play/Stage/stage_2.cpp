#include "stage_2.h"

#include "../Objects/cube.h"
#include "../Objects/fall_box.h"
#include "../Objects/goal_flag.h"
#include "../Player/player.h"
#include "../Player/player_camera.h"
#include "../UI/clear.h"
#include "../UI/moveing_explanation.h"
void Stage2::OnEnter()
{
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Cube>(Vec3(0,0,0),Vec3(2,1,2));
    SpawnActor<Cube>(Vec3(0,0,10),Vec3(2,1,2));
    const Vec3 goal_pos(0,1,20);
    SpawnActor<Cube>(goal_pos,Vec3(2,1,2));
    SpawnActor<GoalFlag>(goal_pos + Vec3(0,-1.0f,0));
    SpawnActor<Explanation>(L"Assets/Texture/explanationjamp.png");
    SpawnActor<Clear>();
    SpawnActor<FallBox>();
    LevelBase::OnEnter();
}

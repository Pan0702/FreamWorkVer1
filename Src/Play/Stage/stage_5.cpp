#include "stage_5.h"
#include "../Objects/cube.h"
#include "../Objects/fall_box.h"
#include "../Objects/goal_flag.h"
#include "../Objects/moving_cube.h"
#include "../Player/player.h"
#include "../Player/player_camera.h"
#include "../UI/clear.h"
#include "../../Resource/audio_manager.h"
#include "../../Core/common_data.h"

void Stage5::OnEnter()
{
    AudioManager::GetInstance().Play(Sound::kGame);
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Cube>(Vec3(0,0,0),Vec3(3,1,3));
    SpawnActor<Cube>(Vec3(0,10,15),Vec3(3,1,3));
    SpawnActor<GoalFlag>(Vec3(0,10,15));
    SpawnActor<Clear>();
    SpawnActor<FallBox>();
    SpawnActor<MovingCube>(Vec3(0,0,5),Vec3(0,10,5));
    LevelBase::OnEnter();
}

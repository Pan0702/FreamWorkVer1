#include "stage_1.h"

#include "../Objects/cube.h"
#include "../Objects/fall_box.h"
#include "../Objects/goal_flag.h"
#include "../Objects/moving_cube.h"
#include "../Player/player.h"
#include "../Player/player_camera.h"
#include "../UI/clear.h"
#include "../UI/moveing_explanation.h"
#include "../../Resource/audio_manager.h"
#include "../../Core/common_data.h"

void  Stage1::OnEnter()
{
    AudioManager::GetInstance().Play(Sound::kGame);
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Cube>(Vec3(0,0,5),Vec3(10,1,20));
    SpawnActor<GoalFlag>(Vec3(0,0,15));
    SpawnActor<MovingCube>(Vec3(0,0,5),Vec3(0,10,5));
    // SpawnActor<Explanation>(L"Assets/Texture/explanation.png");
    SpawnActor<Clear>();
    SpawnActor<FallBox>();
    LevelBase::OnEnter();
}

#include "stage_4.h"
#include "../Objects/cube.h"
#include "../Objects/fall_box.h"
#include "../Objects/goal_flag.h"
#include "../Player/player.h"
#include "../Player/player_camera.h"
#include "../UI/clear.h"
#include "../../Resource/audio_manager.h"
#include "../../Core/common_data.h"

void Stage4::OnEnter()
{
    AudioManager::GetInstance().Play(Sound::kGame);
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    const Vec3 goal_pos(0,1,45);
    SpawnActor<Cube>("Assets/Mesh/Stage2.mesh");
    SpawnActor<GoalFlag>(goal_pos);

    SpawnActor<Clear>();
    SpawnActor<FallBox>();
    LevelBase::OnEnter();
}

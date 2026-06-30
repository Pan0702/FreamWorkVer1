#include "play_level.h"

#include "../Objects/cube.h"
#include "../Objects/fall_box.h"
#include "../Objects/goal_flag.h"
#include "../Player/player.h"
#include "../Player/player_camera.h"
#include "../UI/clear.h"
#include "../UI/moveing_explanation.h"

void PlayLevel::OnEnter()
{
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Cube>();
    SpawnActor<GoalFlag>();
    SpawnActor<MoveingExplanation>();
    SpawnActor<Clear>();
    SpawnActor<FallBox>();

    LevelBase::OnEnter();
}

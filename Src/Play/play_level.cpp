#include "play_level.h"

#include "Objects/cube.h"
#include "Objects/goal_flag.h"
#include "Player/player.h"
#include "Player/player_camera.h"

PlayLevel::PlayLevel()
{

}

void PlayLevel::OnEnter()
{
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Cube>();
    SpawnActor<GoalFlag>();
    LevelBase::OnEnter();
}

#include "play_level.h"

#include "Objects/cube.h"
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
    LevelBase::OnEnter();
}

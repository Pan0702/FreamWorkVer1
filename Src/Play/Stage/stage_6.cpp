#include "stage_6.h"

#include "../../Core/common_data.h"
#include "../../Resource/audio_manager.h"
#include "../Objects/cube.h"
#include "../Objects/fall_box.h"
#include "../Objects/goal_flag.h"
#include "../Objects/moving_cube.h"
#include "../Player/player.h"
#include "../Player/player_camera.h"
#include "../UI/clear.h"

namespace
{
    const char* kStageMesh = "Assets/Mesh/Stage3.mesh";
    const Vec3  kGoalPos   = Vec3(0, 7, 75);

    // 動く床の速度
    constexpr float kSlowSpeed     = 2.0f;
    constexpr float kFastSpeed     = 6.0f;
    constexpr float kVeryFastSpeed = 12.0f;

    // 動く床の配置（始点・終点・速度）
    struct MovingCubeDef { Vec3 from; Vec3 to; float speed; };
    const MovingCubeDef kMovingCubes[] = {
        { Vec3(0,  0, 16), Vec3(0, 7, 16), kSlowSpeed     },
        { Vec3(-6, 7, 34), Vec3(6, 7, 34), kSlowSpeed     },
        { Vec3(-6, 7, 52), Vec3(6, 7, 52), kFastSpeed     },
        { Vec3(-6, 7, 62), Vec3(6, 7, 62), kVeryFastSpeed },
    };
}

void Stage6::OnEnter()
{
    AudioManager::GetInstance().Play(Sound::kGame);
    SpawnActor<Player>();
    SpawnActor<PlayerCamera>();
    SpawnActor<Cube>(kStageMesh);
    SpawnActor<GoalFlag>(kGoalPos);
    SpawnActor<Clear>();
    SpawnActor<FallBox>();

    for (const auto& d : kMovingCubes)
    {
        SpawnActor<MovingCube>(d.from, d.to)->SetSpeed(d.speed);
    }

    LevelBase::OnEnter();
}

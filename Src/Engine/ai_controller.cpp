#include "ai_controller.h"
#include "character.h"
#include "Components/navigation_agent_component.h"

namespace
{
    // 経路を計算し直す間隔(秒)。
    // 短いほど追跡が機敏になるが、経路探索はNavMesh全体を走査するため負荷が上がる。
    constexpr float kRepathInterval = 0.3f;
    // 追跡対象が前回の目的地からこれだけ離れたら、間隔を待たずに経路を引き直す。
    // 間隔だけに任せると、対象が急に動いたときの追従が遅れる。
    constexpr float kRepathDistance = 2.0f;
    // 追跡対象にこれだけ近づいたら移動をやめる。
    // 0にすると対象に重なろうとして、押し合いでガタガタ振動する。
    constexpr float kAcceptanceRadius = 1.5f;
    // ウェイポイントに到達したとみなす距離。
    // 小さすぎると通り過ぎてから戻ろうとし、大きすぎると角を大きく膨らんで曲がる。
    constexpr float kWaypointReachRadius = 0.5f;
}

void AiController::MoveToActor(Actor* actor)
{
    target_ = actor;
    has_goal_ = false;   // 次のTickで必ず経路を引くようにする

    // 経路の始点は所有アクターの位置になるため、Controller自身ではなく
    // 憑依しているCharacterへ取り付ける必要がある。
    if (GetCharacter() != nullptr && nav_agent_ == nullptr)
    {
        nav_agent_ = GetCharacter()->AddComponent<NavigationAgentComponent>();
        nav_agent_->SetDebugDrawEnabled(false);
    }
}

void AiController::StopMovement()
{
    target_ = nullptr;
    has_goal_ = false;
    if (nav_agent_ != nullptr)
    {
        nav_agent_->ClearPath();
    }
}

void AiController::Tick(float dt)
{
    if (target_ == nullptr || GetCharacter() == nullptr || nav_agent_ == nullptr)
    {
        return;
    }

    repath_timer_ += dt;

    const Vec3 target_pos = target_->GetTransform().position;

    // 対象がどれだけ動いたかはXZ平面で測る。
    // 高さを含めると、対象がジャンプするたびに経路を引き直してしまう。
    const float dx = target_pos.x - last_goal_.x;
    const float dz = target_pos.z - last_goal_.z;
    const bool target_moved = (dx * dx + dz * dz) > kRepathDistance * kRepathDistance;

    if (!has_goal_ || repath_timer_ >= kRepathInterval || target_moved)
    {
        // 探索に失敗した場合は last_goal_ を更新しない。
        // 更新すると、対象がNavMesh外にいる間ずっと再探索しなくなる。
        if (nav_agent_->SetDestination(target_pos))
        {
            last_goal_ = target_pos;
            has_goal_ = true;
        }
        repath_timer_ = 0.0f;
    }

    Character* chara = GetCharacter();
    const Vec3 my_pos = chara->GetTransform().position;

    // 経路の終点ではなく対象本体との距離で止める。
    // 経路は再計算までの間、古い位置を指していることがあるため。
    const float tdx = target_pos.x - my_pos.x;
    const float tdz = target_pos.z - my_pos.z;
    if (tdx * tdx + tdz * tdz < kAcceptanceRadius * kAcceptanceRadius)
    {
        return;
    }

    nav_agent_->AdvanceWaypointIfReached(my_pos, kWaypointReachRadius);

    Vec3 waypoint;
    if (!nav_agent_->TryGetCurrentWaypoint(waypoint))
    {
        return;   // 経路なし/走破済み。次の再パスまで待機
    }

    // 高さの差は重力と接地処理に任せるため、水平方向だけを移動入力にする。
    Vec3 dir = waypoint - my_pos;
    dir.y = 0.0f;
    if (dir.LengthSquared() > kEpsilon)
    {
        chara->AddMovementInput(dir.Normalized());
    }

    Controller::Tick(dt);
}

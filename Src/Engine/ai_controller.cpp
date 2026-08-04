#include "ai_controller.h"
#include "character.h"
#include "Components/navigation_agent_component.h"

void AiController::MoveToActor(Actor* actor)
{
    target_ = actor;
    has_goal_ = false;   
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
    
    const float dx = target_pos.x - last_goal_.x;
    const float dz = target_pos.z - last_goal_.z;
    constexpr float kRepathDistance = 2.0f;
    const bool target_moved = (dx * dx + dz * dz) > kRepathDistance * kRepathDistance;

    constexpr float kRepathInterval = 0.3f;
    if (!has_goal_ || repath_timer_ >= kRepathInterval || target_moved)
    {
        if (nav_agent_->SetDestination(target_pos))
        {
            last_goal_ = target_pos;   // ← 成功したときだけ更新
            has_goal_ = true;
        }
        repath_timer_ = 0.0f;
    }
    
    Character* chara = GetCharacter();
    const Vec3 my_pos = chara->GetTransform().position;
    
    const float tdx = target_pos.x - my_pos.x;
    const float tdz = target_pos.z - my_pos.z;
    constexpr float kAcceptanceRadius = 1.5f;
    if (tdx * tdx + tdz * tdz < kAcceptanceRadius * kAcceptanceRadius)
    {
        return;
    }
    
    nav_agent_->AdvanceWaypointIfReached(my_pos, 0.5f);

    Vec3 waypoint;
    if (!nav_agent_->TryGetCurrentWaypoint(waypoint))
    {
        return;   // 経路なし/走破済み。次の再パスまで待機
    }

    Vec3 dir = waypoint - my_pos;
    dir.y = 0.0f;
    if (dir.LengthSquared() > kEpsilon)
    {
        chara->AddMovementInput(dir.Normalized());
    }
    
    Controller::Tick(dt);
}

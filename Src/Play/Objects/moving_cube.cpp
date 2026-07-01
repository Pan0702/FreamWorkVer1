#include "moving_cube.h"

#include <cmath>

#include "cube.h"
#include "../Player/player.h"
#include "../../Engine/Components/static_mesh_component.h"

MovingCube::MovingCube()
{
    Init();
    base_pos_ = Vec3(0.0f, 0.0f, 0.0f);
    target_pos_ = base_pos_ + Vec3(0.0f, 10.0f, 0.0f);
    transform_.position = base_pos_;
    transform_.scale = Vec3(3.0f, 1.0f, 3.0f);
}

MovingCube::MovingCube(const Vec3& pos, const Vec3& target)
{
    Init();
    base_pos_ = pos;
    target_pos_ = target;
    transform_.position = pos;
    transform_.scale = Vec3(2.0f, 1.0f, 2.0f);
}

void MovingCube::Init()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/cube.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
}

// 接触イベントではなく位置で乗車判定する。
// これなら下降中に接触が切れても carry が途切れない。
bool MovingCube::IsRiding(const Vec3& p) const
{
    // cube.mesh はローカルで -1..1 想定なので、ワールド半径 = scale とみなす。
    // メッシュのサイズが違う場合はここを調整する。
    const Vec3 c = transform_.position;
    const Vec3 half = transform_.scale;
    const float top = c.y;
    const float kFootTolerance = 1.0f; // 天面付近とみなす許容
    if (std::fabs(p.x - c.x) > half.x) return false;
    if (std::fabs(p.z - c.z) > half.z) return false;
    if (p.y < top - kFootTolerance || p.y > top + kFootTolerance) return false;
    return true;
}

void MovingCube::Tick(float dt)
{
    const Vec3 seg = target_pos_ - base_pos_;
    const float dist = seg.Length();

    if (waiting_time_ < kWaitingTimeMax)
    {
        waiting_time_ += dt;
    }
    else if (dist > kEpsilon)
    {
        t_ += dir_ * (speed_ / dist) * dt;
        if (t_ >= 1.0f)
        {
            t_ = 1.0f;
            dir_ = -1.0f;
            waiting_time_ = 0.0f;
        }
        else if (t_ <= 0.0f)
        {
            t_ = 0.0f;
            dir_ = 1.0f;
            waiting_time_ = 0.0f;
        }
    }

    const Vec3 new_pos = base_pos_ + seg * t_;
    const Vec3 delta = new_pos - transform_.position; // このフレームの移動量
    transform_.position = new_pos;

    // 乗っていれば水平ぶんだけ運ぶ。縦の追従はプレイヤー側のスナップが担当。
    if (player_ == nullptr)
    {
        player_ = GetWorld()->FindActor<Player>();
    }
    if (player_ != nullptr && IsRiding(player_->GetTransform().position))
    {
        player_->GetTransform().position += Vec3(delta.x, 0.0f, delta.z);
    }

    Actor::Tick(dt);
}

void MovingCube::SetSpeed(float speed)
{
    speed_ = speed;
}

#include "Components/skeletal_mesh.h"
#include "Components/animation_component.h"
#include "Components/capsule_collider_comp.h"
#include "world.h"
#include "../Core/Math/intersect.h"

#include "character.h"
// kJumpVel / kGravityUp / kFallMul の定義元。Engine側へ移すならこの依存を切る。
#include "../Play/Player/player_common.h"
#include "Components/static_mesh_component.h"

namespace
{
    // アニメーションを切り替えるときの補間時間(秒)。
    // 短いと切り替わりが硬く、長いと入力への反応が鈍く見える。
    constexpr float kAnimationCrossFadeTime = 0.2f;
    // 接地判定のレイを足元から持ち上げる高さ。
    // 段差を登った直後など、床に少しめり込んだ状態でも床を拾えるようにする。
    constexpr float kGroundProbeUp = 0.5f;
    // 接地とみなす、足元から床までの許容すき間。
    // 0にすると坂を下るときに接地と空中が交互に切り替わってしまう。
    constexpr float kGroundProbeMargin = 0.1f;
    // メッシュからサイズを取れない場合のカプセルの高さの半分。
    // 派生クラスが SetCapsuleHalfSize で上書きする前提の暫定値。
    constexpr float kDefaultCapsuleHalfHeight = 1.0f;
}

Character::Character(const std::string& mesh_path)
{
    // 拡張子で見た目の作り方を分ける。スケルタルならアニメーションも持たせる。
    const bool is_skeletal = mesh_path.ends_with(".skmesh");
    if (is_skeletal)
    {
        SkeletalMesh* sk = SkeletalMeshManager::Get().Load(mesh_path);
        materials_ = std::make_unique<MaterialSlot>(sk->GetMaterialDecs());
        AddComponent<SkeletalMeshComponent>(sk, materials_.get());
        animation_ = AddComponent<AnimationComponent>();
        SetupCapsule(sk);                       // メッシュからサイズ取得
    }
    else
    {
        Mesh* mesh = MeshManager::Get().Load(mesh_path);
        materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDesc());
        AddComponent<StaticMeshComponent>(mesh, materials_.get());
        // 静的メッシュからはカプセルの寸法を決められないので既定値で作る。
        SetupCapsule(nullptr);
        capsule_->SetHalfSize(Vec3(radius_, kDefaultCapsuleHalfHeight, radius_));
    }
}

void Character::AddMovementInput(const Vec3& world_direction, float scale)
{
    // 実際に動かすのは Tick 側。ここでは足し込むだけなので、
    // 1フレームに複数回呼ばれても合成された1つの入力として扱われる。
    control_input_ += world_direction * scale;
}

void Character::Jump()
{
    // 空中での二段ジャンプと、ジャンプ中の再入力による上昇のやり直しを防ぐ。
    if (is_grounded_ && !pressed_jump_)
    {
        pressed_jump_ = true;
        jump_vel_y_ = kJumpVel;
    }
}

void Character::StopJumping()
{
    pressed_jump_ = false;
}

void Character::Tick(float dt)
{
    if (!use_tick_)
    {
        return;
    }

    // 接地を先に確定させてから移動を解決する。順番を入れ替えると、
    // 着地したフレームに落下速度が乗ったままになり床にめり込む。
    UpdateGroundProbe();

    const Vec3 frame_move = ConsumeMovementInput(dt);

    UpdateLocomotionAnimation(frame_move, pressed_jump_);
    was_grounded_ = is_grounded_;

    Actor::Tick(dt);
}

void Character::UpdateGroundProbe()
{
    // 足元へ下向きのレイを撃ち、このフレームの重力処理より前に接地を確定する。
    Ray ray;
    ray.origin = transform_.position + Vec3(0.0f, kGroundProbeUp, 0.0f);
    ray.direction = Vec3(0.0f, -1.0f, 0.0f);
    ray.distance = kGroundProbeUp + kGroundProbeMargin;
    ContactInfo hit;
    is_grounded_ = GetWorld()->GetCollisionWorld().Raycast(ray, hit);
    if (is_grounded_)
    {
        // 床の高さへ吸着させ、落下速度を捨てる。
        transform_.position.y = ray.origin.y - (hit.normal * hit.depth).y;
        vel_.y = 0.0f;
    }
}

Vec3 Character::ConsumeMovementInput(float dt)
{
    // 蓄積された入力を正規化して水平移動量を作る。
    // 正規化するので、斜め入力で速度が速くなることはない。
    Vec3 move;
    if (control_input_.LengthSquared() > kEpsilon)
    {
        const Vec3 dir = control_input_.Normalized();
        move.x = dir.x * move_speed_ * dt;
        move.z = dir.z * move_speed_ * dt;
        transform_.rotation.y = atan2f(dir.x, dir.z); // 進行方向へ体を向ける
    }

    // ジャンプの上昇。速度が0以下になった時点が頂点で、以降は下の落下処理に任せる。
    if (pressed_jump_)
    {
        if (jump_vel_y_ <= 0.0f)
        {
            pressed_jump_ = false;
        }
        else
        {
            jump_vel_y_ -= kGravityUp * dt;
            move.y += jump_vel_y_ * dt;
        }
    }

    // ジャンプ入力がない空中では落下速度を積分する。
    // 落下時は kFallMul で重力を弱め、上昇より緩やかに落ちるようにしている。
    if (!pressed_jump_ && !is_grounded_)
    {
        vel_.y -= kGravityUp * kFallMul * dt;
        move += vel_ * dt;
    }
    else
    {
        vel_.y = 0.0f;
    }

    transform_.position += move;
    // 消費したので空にする。次フレームに入力がなければ移動量は0になる。
    control_input_ = Vec3();
    return move;
}

void Character::UpdateLocomotionAnimation(const Vec3& frame_move, bool jumping)
{
    if (animation_ == nullptr)
    {
        return;   // 見た目なし/StaticMeshキャラはアニメ選択なし
    }

    const bool just_landed = is_grounded_ && !was_grounded_;
    const bool landing_playing =
        (animation_name_ == CharaAnim::kLanding && animation_->IsPlaying());

    // 上から順に優先度が高い。着地は再生し終わるまで他へ移らないよう、
    // 着地した瞬間だけでなく再生中も選び続ける。
    std::string anim_name;
    if (jumping) anim_name = CharaAnim::kJump;
    else if (!is_grounded_) anim_name = CharaAnim::kFall;
    else if (just_landed) anim_name = CharaAnim::kLanding;
    else if (landing_playing) anim_name = CharaAnim::kLanding;
    else if (frame_move.LengthSquared() > kEpsilon) anim_name = CharaAnim::kRun;
    else anim_name = CharaAnim::kIdle;

    if (anim_name != animation_name_ && !anim_name.empty())
    {
        animation_name_ = anim_name;
        animation_->CrossFade(anim_name, kAnimationCrossFadeTime);
    }
}

void Character::OnHit(ColliderComponent* /*self*/, Actor* /*other_actor*/,
                      ColliderComponent* /*other_coll*/, const ContactInfo& info)
{
    // 押し戻しのみ行い、接地判定はTick冒頭のレイに任せる。
    transform_.position += info.normal * info.depth;
}

void Character::SetupCapsule(const SkeletalMesh* mesh_or_null)
{
    capsule_ = AddComponent<CapsuleColliderComponent>(mesh_or_null);
    capsule_->SetOnHit(this, &Character::OnHit);
    capsule_->SetUseTransform(true);
}

void Character::SetCapsuleHalfSize(const Vec3& half_size)
{
    capsule_->SetHalfSize(half_size);
}

bool Character::IsGrounded() const
{
    return is_grounded_;
}

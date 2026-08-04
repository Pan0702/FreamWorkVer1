#include "Components/skeletal_mesh.h"
#include "Components/animation_component.h"
#include "Components/capsule_collider_comp.h"
#include "world.h"
#include "../Core/Math/intersect.h"

// 元 player_common.h の重力定数。Engine側へ移すならこのヘッダに定義を移動する。
#include "character.h"
#include "../Play/Player/player_common.h" 
#include "Components/static_mesh_component.h"

Character::Character(const std::string& mesh_path)
{
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
        SetupCapsule(nullptr);                  // カプセルはデフォルト構築
        capsule_->SetHalfSize(Vec3(radius_, 1.0f, radius_)); // 派生がSetCapsuleHalfSizeで上書き
    }
}

void Character::AddMovementInput(const Vec3& world_direction, float scale)
{
    control_input_ += world_direction * scale;
}

void Character::Jump()
{
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

    UpdateGroundProbe();

    const Vec3 frame_move = ConsumeMovementInput(dt);

    UpdateLocomotionAnimation(frame_move, pressed_jump_);
    was_grounded_ = is_grounded_;

    Actor::Tick(dt);
}

void Character::UpdateGroundProbe()
{
    // 元 Player::Tick 60-74行。足元へレイを撃ち、このフレームの重力処理前に接地を確定する。
    constexpr float kProbeMargin = 0.1f;
    constexpr float kProbeUp = 0.5f;
    Ray ray;
    ray.origin = transform_.position + Vec3(0.0f, kProbeUp, 0.0f);
    ray.direction = Vec3(0.0f, -1.0f, 0.0f);
    ray.distance = kProbeUp + kProbeMargin;
    ContactInfo hit;
    is_grounded_ = GetWorld()->GetCollisionWorld().Raycast(ray, hit);
    if (is_grounded_)
    {
        transform_.position.y = ray.origin.y - (hit.normal * hit.depth).y;
        vel_.y = 0.0f;
    }
}

Vec3 Character::ConsumeMovementInput(float dt)
{
    // 蓄積された入力を正規化して水平移動量を作る 
    Vec3 move;
    if (control_input_.LengthSquared() > kEpsilon)
    {
        const Vec3 dir = control_input_.Normalized();
        move.x = dir.x * move_speed_ * dt;
        move.z = dir.z * move_speed_ * dt;
        transform_.rotation.y = atan2f(dir.x, dir.z); // 進行方向へ体を向ける
    }

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
    control_input_ = Vec3();
    return move;
}

void Character::UpdateLocomotionAnimation(const Vec3& frame_move, bool jumping)
{
    if (animation_ == nullptr)
    {
        return;   // 見た目なし/StaticMeshキャラはアニメ選択なし
    }

    // 元 Player::Tick 103-123行。
    const bool just_landed = is_grounded_ && !was_grounded_;
    const bool landing_playing =
        (animation_name_ == CharaAnim::kLanding && animation_->IsPlaying());

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
        animation_->CrossFade(anim_name, 0.2f);
    }
}

void Character::OnHit(ColliderComponent* /*self*/, Actor* /*other_actor*/,
                      ColliderComponent* /*other_coll*/, const ContactInfo& info)
{
    //押し戻しのみ行い、接地判定はTick冒頭のレイに任せる。
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

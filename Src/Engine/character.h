#pragma once
#include "actor.h"
#include "../Resource/material_slot.h"
#include <string>
#include <memory>

class AnimationComponent;
class ColliderComponent;
class SkeletalMesh;
class CapsuleColliderComponent;
struct ContactInfo;

// Characterが参照するアニメーション名の規約。派生はこの名前で登録する。
namespace CharaAnim
{
    inline const std::string kRun     = "run";
    inline const std::string kIdle    = "idle";
    inline const std::string kJump    = "jump";
    inline const std::string kFall    = "fall";
    inline const std::string kLanding = "landing";
}

/**
 * @brief 二足歩行キャラクターの共通基盤。カプセル衝突・接地・重力・
 *        移動入力の消費・ロコモーションアニメ選択を担う。
 *        入力デバイスやAIは知らない。ControllerがAddMovementInputで命令する。
 */
class Character : public Actor
{
public:
    explicit Character(const std::string& mesh_path);
    
    void AddMovementInput(const Vec3& world_direction, float scale = 1.0f);
    void Jump();
    void StopJumping();

    bool IsGrounded() const ;

protected:
    void Tick(float dt) override;
    void SetCapsuleHalfSize(const Vec3& half_size);   // StaticMesh/見た目なし派生用

    AnimationComponent* animation_ = nullptr;   // アニメ登録は派生が行う
    float move_speed_ = 10.0f;                  // 派生が調整可
    float radius_ = 0.5f;

private:
    void UpdateGroundProbe();                   
    Vec3 ConsumeMovementInput(float dt);        
    void UpdateLocomotionAnimation(const Vec3& frame_move, bool jumping); 
    void OnHit(ColliderComponent* self, Actor* other_actor,
               ColliderComponent* other_coll, const ContactInfo& info);   // 押し戻し 
    void SetupCapsule(const SkeletalMesh* mesh_or_null);  

    std::unique_ptr<MaterialSlot> materials_;
    CapsuleColliderComponent* capsule_ = nullptr;  
    Vec3 control_input_;        // AddMovementInputの蓄積。消費後ゼロクリア
    Vec3 vel_;
    std::string animation_name_;
    float jump_vel_y_ = 0.0f;
    bool pressed_jump_ = false; // Jump()で立ちStopJumping()/着地で降ろす
    bool is_grounded_ = true;
    bool was_grounded_ = true;
};
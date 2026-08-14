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

/**
 * @brief Character が参照するアニメーション名の規約。
 *
 * ロコモーションの選択は Character 側が行うため、派生クラスは
 * ここにある名前でアニメーションを登録しなければならない。
 */
namespace CharaAnim
{
    inline const std::string kRun     = "run";
    inline const std::string kIdle    = "idle";
    inline const std::string kJump    = "jump";
    inline const std::string kFall    = "fall";
    inline const std::string kLanding = "landing";
}

/**
 * @brief 二足歩行キャラクターの共通基盤。
 *
 * カプセル衝突、接地判定、重力、移動入力の消費、ロコモーションアニメの
 * 選択を担う。入力デバイスや AI は知らないため、動かす側(Controller)が
 * AddMovementInput と Jump で命令する。
 */
class Character : public Actor
{
public:
    /**
     * @brief メッシュを読み込み、カプセルコライダーと描画用コンポーネントを構築する。
     * @param mesh_path 読み込むメッシュのパス。拡張子が .skmesh ならスケルタル
     *                  メッシュとアニメーションを、それ以外なら静的メッシュを使う。
     */
    explicit Character(const std::string& mesh_path);

    /**
     * @brief このフレームの移動入力を加算する。
     *
     * 加算された入力は Tick 内で消費され、消費後はゼロに戻る。
     * 1 フレーム中に複数回呼び出した場合はベクトルの和として合成される。
     * @param world_direction ワールド空間での移動方向。
     * @param scale world_direction に掛ける倍率。
     */
    void AddMovementInput(const Vec3& world_direction, float scale = 1.0f);
    /**
     * @brief 接地中であればジャンプを開始する。
     */
    void Jump();
    /**
     * @brief ジャンプ入力を解除して落下へ移らせる。
     */
    void StopJumping();

    /**
     * @brief 直近の接地判定の結果を取得する。
     * @return 接地している場合は true。
     */
    bool IsGrounded() const ;

protected:
    /**
     * @brief 接地判定、移動入力の消費、アニメーション選択を 1 フレーム分進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief カプセルコライダーの半サイズを上書きする。
     *
     * スケルタルメッシュからサイズを取得できない派生クラスが使う。
     * @param half_size XZ を半径、Y を高さの基準として使う半サイズ。
     */
    void SetCapsuleHalfSize(const Vec3& half_size);

    AnimationComponent* animation_ = nullptr;   // アニメ登録は派生が行う
    std::unique_ptr<MaterialSlot> materials_;
    float move_speed_ = 10.0f;                  // 派生が調整可
    float radius_ = 0.5f;

private:
    /**
     * @brief 足元へレイを撃ち、重力処理より前に接地状態を確定させる。
     */
    void UpdateGroundProbe();
    /**
     * @brief 蓄積された移動入力を消費し、重力と合成して位置と向きを更新する。
     * @param dt 前フレームからの経過秒数。
     * @return このフレームで実際に動いた量。
     */
    Vec3 ConsumeMovementInput(float dt);
    /**
     * @brief 接地状態と移動量から再生するアニメーションを選び、切り替える。
     * @param frame_move このフレームの移動量。
     * @param jumping ジャンプ中であれば true。
     */
    void UpdateLocomotionAnimation(const Vec3& frame_move, bool jumping);
    /**
     * @brief コライダー接触時に、接触法線の方向へ押し戻す。
     * @param self 接触したこちら側のコライダー。
     * @param other_actor 接触相手のアクター。
     * @param other_coll 接触相手のコライダー。
     * @param info 接触法線と貫通量。
     */
    void OnHit(ColliderComponent* self, Actor* other_actor,
               ColliderComponent* other_coll, const ContactInfo& info);
    /**
     * @brief カプセルコライダーを生成し、接触コールバックを登録する。
     * @param mesh_or_null カプセルサイズの基にするスケルタルメッシュ。
     *                     nullptr の場合は既定サイズで構築する。
     */
    void SetupCapsule(const SkeletalMesh* mesh_or_null);


    CapsuleColliderComponent* capsule_ = nullptr;
    Vec3 control_input_;        // AddMovementInputの蓄積。消費後ゼロクリア
    Vec3 vel_;
    std::string animation_name_;
    float jump_vel_y_ = 0.0f;
    bool pressed_jump_ = false; // Jump()で立ちStopJumping()/着地で降ろす
    bool is_grounded_ = true;
    bool was_grounded_ = true;
};

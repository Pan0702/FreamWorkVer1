#pragma once
#include "../../Engine/actor.h"
#include "../../Resource/material_slot.h"
#include "PlayerComponent/component_factor.h"
class StateComponentBase;

// プレイヤーが同時に持てる行動状態のビット。//
enum class PlayerState : uint8_t
{
    kIdle = 1 << 0,
    kWalk = 1 << 1,
    kJump = 1 << 2,
};

/**
 * @brief PlayerState を状態管理用のビット値へ変換する。
 * @param state ビットへ変換するプレイヤー状態。
 * @return state に対応するビット値。
 */
inline uint32_t Bit(const PlayerState& state)
{
    return static_cast<uint32_t>(state);
}

/**
 * @brief 状態ビット集合に指定したプレイヤー状態が含まれるか調べる。
 * @param bit 複数の PlayerState を OR した状態ビット集合。
 * @param state 含まれているか調べるプレイヤー状態。
 * @return bit に state が含まれている場合は true。
 */
inline bool IsSet(uint32_t bit, const PlayerState& state)
{
    return (bit & Bit(state)) != 0;
}

// 1 フレーム分のプレイヤー入力と、そこから計算した移動量をまとめる。//
struct PlayerInput
{
    Vec3 move_amount = {0.0f, 0.0f, 0.0f};
    Vec3 move_dir = {0.0f, 0.0f, 0.0f};
    float yaw = 0.0f;
    bool jump = false;
};


// プレイヤー本体の描画、入力、状態遷移、当たり判定反応をまとめる。//
class Player : public Actor
{
public:
    /**
     * @brief プレイヤーのメッシュ、アニメーション、カプセルコライダーを準備する。
     */
    Player();

private:
    /**
     * @brief アクター生成直後の開始処理を行う。
     */
    void Begin() override;
    /**
     * @brief 入力、状態コンポーネント、重力、姿勢を 1 フレーム分更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief キー入力と接地状態から移動量とジャンプ入力を作る。
     * @param dt 前フレームからの経過秒数。
     * @return このフレームの移動量、向き、ジャンプ状態。
     */
    PlayerInput Input(float dt);
    /**
     * @brief 入力内容から実行したいプレイヤー状態ビットを組み立てる。
     * @param in このフレームの入力内容。
     * @return 入力内容から作成した PlayerState のビット集合。
     */
    uint32_t BuildWantedState(const PlayerInput& in) const;
    /**
     * @brief 前フレームとの差分を見て状態コンポーネントの開始・終了を呼び分ける。
     * @param new_bits 次に有効にする PlayerState のビット集合。
     */
    void SetState(uint32_t new_bits);
    /**
     * @brief コライダー接触時にプレイヤーを接触法線方向へ押し戻す。
     * @param self 接触したプレイヤー側のコライダー。
     * @param other_actor 接触相手のアクター。
     * @param other_coll 接触相手のコライダー。
     * @param info 接触法線と貫通量。
     */
    void OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info);
    std::unique_ptr<MaterialSlot> materials_;
    class AnimationComponent* animation_ = nullptr;

    Vec3 vel_;
    uint32_t state_bit_ = 0;
    ComponentFactor factor_;
    std::unordered_map<PlayerState, StateComponentBase*> states_;
    PlayerInput pl_input_;
    bool is_moving_ = false;
    bool is_grounded_ = true;
    bool was_grounded_ = true;
    std::string animation_name_;
};
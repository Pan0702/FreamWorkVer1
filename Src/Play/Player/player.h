#pragma once
#include "../../Engine/actor.h"
#include "../../Resource/material_slot.h"
#include "PlayerComponent/component_factor.h"
class StateComponentBase;

enum class PlayerState : uint8_t
{
    kIdle = 1 << 0,
    kWalk = 1 << 1,
    kJump = 1 << 2,
};

/**
 * @brief 入力フラグ用のビット値を作る。
 * @param state state に設定する値。
 * @return state に対応する入力フラグのビット値。
 */
inline uint32_t Bit(const PlayerState& state)
{
    return static_cast<uint32_t>(state);
}

/**
 * @brief 指定した状態ビットが含まれるか調べる。
 * @param bit bit に設定する値。
 * @param state state に設定する値。
 * @return state に bit が含まれている場合は true。
 */
inline bool IsSet(uint32_t bit, const PlayerState& state)
{
    return (bit & Bit(state)) != 0;
}

// PlayerInput に関係する状態と振る舞いをまとめる型。
struct PlayerInput
{
    Vec3 move_amount = {0.0f, 0.0f, 0.0f};
    Vec3 move_dir = {0.0f, 0.0f, 0.0f};
    float yaw = 0.0f;
    bool jump = false;
};


// Player に関係する状態と振る舞いをまとめる型。
class Player : public Actor
{
public:
    /**
     * @brief インスタンスの初期状態を整える。
     */
    Player();

private:
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void Begin() override;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief プレイヤー操作を読み取り移動状態へ反映する。
     * @param dt 前フレームからの経過秒数。
     * @return 入力から組み立てた移動方向とアクション状態。
     */
    PlayerInput Input(float dt);
    /**
     * @brief 入力内容から希望する状態ビットを組み立てる。
     * @param in in に設定する値。
     * @return 入力内容から作成した PlayerState のビット集合。
     */
    uint32_t BuildWantedState(const PlayerInput& in) const;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param new_bits new_bits に設定する値。
     */
    void SetState(uint32_t new_bits);
    /**
     * @brief 接触結果をプレイヤー側の反応へ変換する。
     * @param self self に設定する値。
     * @param other_actor other_actor に設定する値。
     * @param other_coll 判定または通知に使用するコライダー。
     * @param info 計算結果を書き込む情報。
     */
    void OnHit(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll, const ContactInfo& info);
    std::unique_ptr<MaterialSlot> materials_;
    class AnimationComponent* animation_ = nullptr;

    Vec3 vel_;
    uint32_t state_bit_ = 0;
    bool is_moving_ = false;
    bool is_grounded_ = true;
    PlayerInput pl_input_;
    ComponentFactor factor_;
    std::unordered_map<PlayerState, StateComponentBase*> states_;
};

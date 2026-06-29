#pragma once
#include <string>
#include <vector>
#include "../Core/Math/my_math.h"
struct SkAnimFileHeader;

template<class T>
/**
 * @brief Keyのデータと処理をまとめる型。
 */
struct Key
{
    float time;
    T value;
};
/**
 * @brief NodeAnimationのデータと処理をまとめる型。
 */
struct NodeAnimation
{
    std::string name;
    std::vector<Key<Vec3>> position_keys;
    std::vector<Key<Quat>> rotation_keys;
    std::vector<Key<Vec3>> scale_keys;   
};
/**
 * @brief Animationのデータと処理をまとめる型。
 */
class Animation
{
public:
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param duration アニメーション全体の長さ。
     * @param ticks_per_second 1 秒あたりのアニメーション tick 数。
     * @param node アニメーションを適用するノード情報。
     */
    void SetData(float duration, float ticks_per_second,
                 std::vector<NodeAnimation> node);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param loop ループ再生するかどうか。
     */
    void SetLooping(bool loop);
    /**
     * @brief 再生時間 を取得する。
     * @return 現在保持している 再生時間。
     */
    float GetDuration() const;
    /**
     * @brief 1 秒あたりのアニメーション tick 数を取得する。
     * @return 1 秒あたりに進むアニメーション tick 数。
     */
    float GetTicksPerSecond() const;
    /**
     * @brief 現在の状態が条件を満たしているか調べる。
     * @return 現在の状態が条件を満たしているか調べる 場合は true。
     */
    bool IsLooping() const;
    /**
     * @brief Node Animations を取得する。
     * @return 保持している Node Animations への参照。
     */
    const std::vector<NodeAnimation>& GetNodeAnimations() const;
private:
    float duration_ = 0.0f;
    float ticks_per_second_ = 0.0f;
    std::vector<NodeAnimation> node_animations_;
    bool loop_ = true;
};

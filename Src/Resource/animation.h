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
     * @brief Dataを設定する関数。
     * @param duration 引数。
     * @param ticks_per_second 引数。
     * @param node 引数。
     */
    void SetData(float duration, float ticks_per_second,
                 std::vector<NodeAnimation> node);
    /**
     * @brief Loopingを設定する関数。
     * @param loop 引数。
     */
    void SetLooping(bool loop);
    /**
     * @brief Durationを取得する関数。
     * @return 戻り値。
     */
    float GetDuration() const;
    /**
     * @brief TicksPerSecondを取得する関数。
     * @return 戻り値。
     */
    float GetTicksPerSecond() const;
    /**
     * @brief Loopingかどうかを確認する関数。
     * @return 条件を満たす場合は true。
     */
    bool IsLooping() const;
    /**
     * @brief NodeAnimationsを取得する関数。
     * @return 戻り値。
     */
    const std::vector<NodeAnimation>& GetNodeAnimations() const;
private:
    float duration_ = 0.0f;
    float ticks_per_second_ = 0.0f;
    std::vector<NodeAnimation> node_animations_;
    bool loop_ = true;
};

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "../../Core/Math/my_math.h"
#include "../component.h"

struct SkeletonNode;
struct NodeAnimation;
class Skeleton;
class Animation;

/**
 * @brief AnimationComponentのデータと処理をまとめる型。
 */
class AnimationComponent : public Component
{
public:
    /**
     * @brief OnAttachを行う関数。
     * @param context 共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    void Tick(float dt) override;
    /**
     * @brief AddAnimationを行う関数。
     * @param name 検索または識別に使う名前。
     * @param clip 引数。
     * @param loop 引数。
     */
    void AddAnimation(const std::string& name, Animation* clip, bool loop);
    /**
     * @brief Playを行う関数。
     * @param name 検索または識別に使う名前。
     */
    void Play(const std::string& name);
    /**
     * @brief CrossFadeを行う関数。
     * @param name 検索または識別に使う名前。
     * @param fade_time 引数。
     */
    void CrossFade(const std::string& name, float fade_time);
    /**
     * @brief Stopを行う関数。
     */
    void Stop();
    /**
     * @brief CurrentFrameを取得する関数。
     * @return 戻り値。
     */
    float GetCurrentFrame() const;
    /**
     * @brief PlaySpeedを設定する関数。
     * @param speed 引数。
     */
    void SetPlaySpeed(float speed);
    /**
     * @brief BonePaletteを取得する関数。
     * @return 戻り値。
     */
    const std::vector<Mat>& GetBonePalette() const;
    /**
     * @brief Playingかどうかを確認する関数。
     * @return 条件を満たす場合は true。
     */
    bool IsPlaying() const;

private:
    /**
     * @brief RebuildChannelsを行う関数。
     */
    void RebuildChannels();

    Skeleton* skeleton_ = nullptr;
    Animation* clip_ = nullptr;
    Animation* prev_clip_ = nullptr;
    float time_ = 0.0f;
    float prev_time_ = 0.0f;
    float fade_duration_ = 0.0f;
    float fade_elapsed_ = 0.0f;
    float play_speed_ = 1.0f;
    std::vector<Mat> global_poses_;
    std::vector<Mat> bone_matrices_;
    std::vector<const NodeAnimation*> node_channels_;
    std::vector<const NodeAnimation*> prev_channels_;
    bool channels_dirty_ = true;
    bool loop_ = true;
    bool playing_ = false;
    bool fading_ = false;
    std::unordered_map<std::string, Animation*> animation_deta_;
};

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
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief 再生候補のアニメーションを登録する。
     * @param name name に設定する値。
     * @param clip clip に設定する値。
     * @param loop loop に設定する値。
     */
    void AddAnimation(const std::string& name, Animation* clip, bool loop);
    /**
     * @brief 指定したアニメーションの再生を開始する。
     * @param name name に設定する値。
     */
    void Play(const std::string& name);
    /**
     * @brief 数学計算の結果を求める。
     * @param name name に設定する値。
     * @param fade_time fade_time に設定する値。
     */
    void CrossFade(const std::string& name, float fade_time);
    /**
     * @brief 再生中のアニメーションを停止する。
     */
    void Stop();
    /**
     * @brief 現在の再生フレームを取得する。
     * @return アニメーション再生位置を表す現在フレーム。
     */
    float GetCurrentFrame() const;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param speed speed に設定する値。
     */
    void SetPlaySpeed(float speed);
    /**
     * @brief Bone Palette を取得する。
     * @return 保持している Bone Palette への参照。
     */
    const std::vector<Mat>& GetBonePalette() const;
    /**
     * @brief 現在の状態が条件を満たしているか調べる。
     * @return 音声またはアニメーションが再生中の場合は true。
     */
    bool IsPlaying() const;

private:
    /**
     * @brief アニメーション再生用のチャンネル対応を作り直す。
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

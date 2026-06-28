#pragma once
#include <memory>
#include <string>

#include "level_base.h"
#include "level_factory.h"
#include "level_transition.h"

/**
 * @brief LevelManagerのデータと処理をまとめる型。
 */
class LevelManager
{
public:
    /**
     * @brief インスタンスの初期状態を整える。
     */
    LevelManager();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~LevelManager();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param world Actor や描画対象を管理する World。
     */
    void Initialize(World* world);
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt);
    /**
     * @brief 次に開く Level を予約する。
     * @param name name に設定する値。
     */
    void OpenLevel(std::string_view name);
private:
    /**
     * @brief 予約された Level へ切り替える。
     */
    void ApplyPendingLevel();
    std::string current_name_;
    std::string next_name_;
    World* world_ = nullptr;
    std::unique_ptr<LevelBase> current_level_ = nullptr;
    LevelFactory level_factory_;
    LevelTransition transition_;
};

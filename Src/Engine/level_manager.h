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
     * @brief 値を初期化する。
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
     * @param name 対象の名前。
     */
    void OpenLevel(std::string_view name);

    /**
     * @brief アニメーションをせずに開く
     * @param name 対象の名前
     */
    void OpenLevelImmediate(std::string_view name);
    /**
     * いまのLevelの名前を返す
     * @return current_name_を返す。
     */
    const std::string& GetCurrentLevelName() const ;
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

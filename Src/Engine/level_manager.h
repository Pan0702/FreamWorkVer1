#pragma once
#include <memory>
#include <string>

#include "level_base.h"
#include "level_factory.h"
/**
 * @brief LevelManagerのデータと処理をまとめる型。
 */
class LevelManager
{
public:
    /**
     * @brief LevelManagerを初期化するコンストラクタ。
     */
    LevelManager();
    /**
     * @brief LevelManagerの終了処理を行うデストラクタ。
     */
    ~LevelManager();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param world 引数。
     */
    void Initialize(World* world);
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    void Tick(float dt);
    /**
     * @brief OpenLevelを行う関数。
     * @param name 検索または識別に使う名前。
     */
    void OpenLevel(const std::string& name);
private:
    /**
     * @brief ApplyPendingLevelを行う関数。
     */
    void ApplyPendingLevel();
    std::string current_name_;
    std::string next_name_;
    World* world_ = nullptr;
    std::unique_ptr<LevelBase> current_level_ = nullptr;
    LevelFactory level_factory_;
};

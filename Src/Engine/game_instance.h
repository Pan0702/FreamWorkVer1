#pragma once
#include "level_manager.h"
#include "world.h"

/**
 * @brief GameInstanceのデータと処理をまとめる型。
 */
class GameInstance
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @return 条件を満たす場合は true。
     */
    bool Initialize();
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    void Tick(float dt);
    /**
     * @brief 登録済みの描画対象を描画する関数。
     */
    void Render();

    /**
     * @brief Worldを取得する関数。
     * @return 戻り値。
     */
    World* GetWorld();
    /**
     * @brief LevelManagerを取得する関数。
     * @return 戻り値。
     */
    LevelManager& GetLevelManager();

private:
    World world_;
    LevelManager level_manager_;
};

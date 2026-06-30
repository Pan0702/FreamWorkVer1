#pragma once
#include "level_manager.h"
#include "world.h"

class Option;
/**
 * @brief GameInstanceのデータと処理をまとめる型。
 */
class GameInstance
{
public:
    GameInstance();
    virtual ~GameInstance();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize();
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    virtual void Tick(float dt);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void Render();

    /**
     * @brief Actor が所属している World を取得する。
     * @return 所属中の World。見つからない、または未作成の場合は nullptr。
     */
    World* GetWorld();
    /**
     * @brief Level Manager を取得する。
     * @return 保持している Level Manager への参照。
     */
    LevelManager& GetLevelManager();

    /**
     * Tick関数を使うかどうか
     * @param use_tick Flagを渡す
     */
    void SetUseTick(bool use_tick);
private:
    World world_;
    LevelManager level_manager_;
    std::unique_ptr<Option> option_;
    bool use_tick_ = true;
};

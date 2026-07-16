#pragma once
#include "../../Engine/level_base.h"

class Test2: public LevelBase
{
public:
    /**
     * @brief 値を初期化する。
     */
    Test2();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~Test2() override;
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void OnEnter() override;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
};

#pragma once
#include "../Engine/level_base.h"
/**
 * @brief レベル選択画面に必要なアクターを生成する。
 */
class SelectLevel : public LevelBase
{
public:
    /**
     * @brief レベル開始後に選択画面を初期化する。
     */
    void OnEnter() override;
};

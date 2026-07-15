#pragma once
#include "../Engine/level_base.h"

/**
 * @brief タイトル画面レベルに必要なアクターを生成する。
 */
class TitleLevel : public LevelBase
{
public:
    /**
     * @brief レベル開始後にタイトル画面を初期化する。
     */
    void OnEnter() override;
};

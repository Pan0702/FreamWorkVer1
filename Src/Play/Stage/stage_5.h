#pragma once
#include "../../Engine/level_base.h"

/**
 * @brief ステージ 5 のアクターと初期設定を定義する。
 */
class Stage5 : public LevelBase
{
public:
    /**
     * @brief レベル開始後にステージのアクターを生成する。
     */
    void OnEnter() override;
};

#pragma once
#include "../../Engine/level_base.h"
/**
 * @brief ステージ 3 のアクターと初期設定を定義する。
 */
class Stage3 : public LevelBase
{
public:
    /**
     * @brief レベル開始後にステージのアクターを生成する。
     */
    void OnEnter() override;
};

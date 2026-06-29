#pragma once
#include "../Engine/level_base.h"
// PlayLevel に関係する状態と振る舞いをまとめる型。
class PlayLevel : public LevelBase
{
public:
    /**
     * @brief 値を初期化する。
     */
    PlayLevel();
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void OnEnter() override;
};

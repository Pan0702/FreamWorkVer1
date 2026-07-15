#pragma once
#include "../../Engine/level_base.h"
// PlayLevel に関係する状態と振る舞いをまとめる型。
class Stage1 : public LevelBase
{
public:
    /**
     * @brief ステージ1を既定の状態で生成する。
     */
     Stage1() = default;
    /**
     * @brief ステージ1のリソースを解放する。
     */
    ~ Stage1() override = default;
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void OnEnter() override;
};

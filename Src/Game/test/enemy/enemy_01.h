#pragma once
#include "../../../Engine/character.h"

/**
 * @brief 追跡動作の確認に使う敵キャラクター。
 *
 * 移動の判断は行わず、AiController から渡される移動入力で動く。
 */
class Enemy01 : public Character
{
public:
    /**
     * @brief メッシュと移動速度を設定する。
     */
    Enemy01();
    /**
     * @brief 初期位置を設定する。
     */
    void Begin() override;
};

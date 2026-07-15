#pragma once
#include "../../Engine/actor.h"

/**
 * @brief プレイヤーを落下またはリセットさせるトリガーを表す。
 */
class FallBox :public Actor
{
public:
    /**
     * @brief 落下トリガーのアクターを生成する。
     */
    FallBox();
    /**
     * @brief コライダーが落下トリガーへ入ったときの処理を行う。
     * @param my このアクターのコライダー。
     * @param other トリガーに入ったコライダー。
     */
    void OnBeginOverlap(const ColliderComponent* my,const ColliderComponent* other);
};

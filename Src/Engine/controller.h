#pragma once
#include "actor.h"
class Character;

/**
 * @brief Character を操作する主体の基底。
 *
 * Character を憑依(Possess)して保持し、派生クラスが入力や AI に応じて
 * その Character へ移動命令を出す。体(Character)と操作(Controller)を
 * 分けることで、同じ Character を人間の操作にも AI にも使える。
 */
class Controller : public Actor
{
public:
    /**
     * @brief 操作対象の Character を憑依する。
     * @param character 操作対象にする Character。
     */
    void Possess(Character* character);
    /**
     * @brief 憑依を解除し、操作対象を持たない状態にする。
     */
    void UnPossess();
    /**
     * @brief 憑依している Character を取得する。
     * @return 憑依中の Character。憑依していない場合は nullptr。
     */
    Character* GetCharacter() const;
private:
    Character* possessed_ = nullptr;
};

#pragma once
#include <vector>

#include "attach_context.h"

class Actor;

/**
 * @brief Worldのデータと処理をまとめる型。
 */
class World
{
public:
    /**
     * @brief RegisterActorを行う関数。
     * @param actor 引数。
     */
    void RegisterActor(Actor* actor);
    /**
     * @brief UnregisterActorを行う関数。
     * @param actor 引数。
     */
    void UnregisterActor(Actor* actor);
    /**
     * @brief AttachContextを設定する関数。
     * @param context 共有コンテキスト。
     */
    void SetAttachContext(const AttachContext& context);
    /**
     * @brief AttachContextを取得する関数。
     * @return 戻り値。
     */
    AttachContext GetAttachContext() const;
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    void Tick(float dt);

private:
    std::vector<Actor*> actors_;
    AttachContext attach_context_ = {};
};

#pragma once
#include <vector>

#include "attach_context.h"
#include "collision_world.h"

/**
 * @brief Worldのデータと処理をまとめる型。
 */
class World
{
public:
    World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) = delete;
    World& operator=(World&&) = delete;
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

    template<class T = Actor>
T* FindActor()
    {
        static_assert(std::is_base_of_v<Actor, T>);
        for (auto& actor : actors_)
        {
            if (auto* result = dynamic_cast<T*>(actor))
            {
                return result;
            }
        }
        return nullptr;
    }
private:
    std::vector<Actor*> actors_;
    AttachContext attach_context_ = {};
    CollisionWorld collision_world_;
};

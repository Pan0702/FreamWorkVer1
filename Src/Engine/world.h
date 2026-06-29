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
    /**
     * @brief 値を初期化する。
     */
    World() = default;
    /**
     * @brief 値を初期化する。
     */
    World(const World&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    World& operator=(const World&) = delete;
    /**
     * @brief 値を初期化する。
     */
    World(World&&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    World& operator=(World&&) = delete;
    /**
     * @brief 対象を管理リストへ登録する。
     * @param actor 登録または解除する Actor。
     */
    void RegisterActor(Actor* actor);
    /**
     * @brief 対象を管理リストから外す。
     * @param actor 登録または解除する Actor。
     */
    void UnregisterActor(Actor* actor);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void SetAttachContext(const AttachContext& context);
    /**
     * @brief Attach Context を取得する。
     * @return 現在保持している Attach Context。
     */
    AttachContext GetAttachContext() const;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt);

    /**
     * @brief CollisionWorld を取得する。
     * @return 保持している CollisionWorld への参照。
     */
    CollisionWorld& GetCollisionWorld() { return collision_world_; }

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

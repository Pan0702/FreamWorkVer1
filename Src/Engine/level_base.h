#pragma once
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>
#include "actor.h"
class World;

/**
 * @brief LevelBaseのデータと処理をまとめる型。
 */
class LevelBase
{
public:
    LevelBase() = default;
    virtual ~LevelBase() = default;
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    virtual void Tick(float dt){}
    /**
     * @brief OnEnterを行う関数。
     */
    virtual void OnEnter(){}
    /**
     * @brief OnExitを行う関数。
     */
    virtual void OnExit(){}
    // world: Actor を所属させるワールド。
    void SetWorld(World* world)
    {
        world_ = world;
    }

    // T: 生成する Actor 派生型。args: コンストラクタ引数。戻り値: 生成した Actor。
    template <class T = Actor, class... Args>
    T* SpawnActor(Args&&... args)
    {
        static_assert(std::is_base_of_v<Actor, T>);
        auto actor = std::make_unique<T>(std::forward<Args>(args)...);
        T* result = actor.get();
        actors_.push_back(std::move(actor));
        result->OnSpawn(world_);
        return result;
    }

protected:
    World* world_ = nullptr;
    std::vector<std::unique_ptr<Actor>> actors_;
};

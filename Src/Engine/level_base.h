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
    /**
     * @brief 値を初期化する。
     */
    LevelBase() = default;
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    virtual ~LevelBase() = default;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    virtual void Tick(float dt){}
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    virtual void OnEnter(){}
    /**
     * @brief 終了または遷移前に必要な後始末を行う。
     */
    virtual void OnExit(){}
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param world Actor や描画対象を管理する World。
     */
    void SetWorld(World* world)
    {
        world_ = world;
    }

    /**
     * @brief Actor 派生型を生成し、World へ登録して所有権を保持する。
     * @param args 生成する型へ渡すコンストラクタ引数。
     * @return 生成して World に登録した Actor。
     */
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

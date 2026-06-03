#pragma once
#include <memory>
#include <vector>
#include <utility>
#include <type_traits>
#include "actor.h"
class World;

class LevelBase
{
public:
    LevelBase();
    virtual ~LevelBase();
    virtual void Tick(float dt){}
    virtual void OnEnter(){}
    virtual void OnExit(){}
    void SetWorld(World* world)
    {
        world_ = world;
    }
    
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

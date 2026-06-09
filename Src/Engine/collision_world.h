#pragma once
#include <functional>
#include <set>
#include <vector>

#include "Components/collider_component.h"

class CollisionWorld
{
public:
    void Collect();
    void Register(ColliderComponent* coll);
    void Unregister(ColliderComponent* coll);
    bool TestCollision(ColliderComponent* coll1, ColliderComponent* coll2);
private:
    using Pair = std::pair<ColliderComponent*, ColliderComponent*>;

    // {a,b} と {b,a} を同一視するため std::less でポインタ順を正規化する。
    static Pair MakePair(ColliderComponent* a, ColliderComponent* b)
    {
        return std::less<ColliderComponent*>{}(a, b) ? Pair{a, b} : Pair{b, a};
    }

    std::vector<ColliderComponent*> colliders_; //Colliderの配列
    std::set<Pair> prev_pairs_;//前フレーム重なってたペア
    std::vector<ColliderComponent*> pending_add_; //登録用の待機列
    std::vector<ColliderComponent*> pending_remove_; //解除用の待機列
    bool dispatching_ = false; //コールバック呼び出しFlag
};

#pragma once
#include <functional>
#include <set>
#include <vector>

#include "Components/collider_component.h"

struct HitPair
{
    ColliderComponent* collider1;
    ColliderComponent* collider2;
};
class CollisionWorld
{
public:
    void Collect();
    void Register(ColliderComponent* coll);
    void Unregister(ColliderComponent* coll);
    bool TestCollision(ColliderComponent* coll1, ColliderComponent* coll2);

private:
    
    std::vector<ColliderComponent*> colliders_; //Colliderの配列
    std::set<HitPair> prev_pairs_;//前フレーム重なってたペア
    std::vector<ColliderComponent*> pending_add_; //登録用の待機列
    std::vector<ColliderComponent*> pending_remove_; //解除用の待機列
    bool dispatching_ = false; //コールバック呼び出しFlag
};

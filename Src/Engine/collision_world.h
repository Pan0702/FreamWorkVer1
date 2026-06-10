#pragma once
#include <functional>
#include <set>
#include <vector>

#include "Components/collider_component.h"
#include "Components/mesh_collider_component.h"

struct Box;
class MeshColliderComponent;
struct Sphere;

struct HitPair
{
    ColliderComponent* collider1;
    ColliderComponent* collider2;

    HitPair(ColliderComponent* a, ColliderComponent* b)
    {
        // 生ポインタの < は未規定。必ず std::less を通す
        if (std::less<ColliderComponent*>{}(a, b))
        {
            collider1 = a;
            collider2 = b;
        }
        else
        {
            collider1 = b;
            collider2 = a;
        }
    }

    bool operator<(const HitPair& o) const
    {
        if (collider1 != o.collider1)
            return std::less<ColliderComponent*>{}(collider1, o.collider1);
        return std::less<ColliderComponent*>{}(collider2, o.collider2);
    }
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
    std::set<HitPair> prev_pairs_; //前フレーム重なってたペア
    std::vector<ColliderComponent*> pending_add_; //登録用の待機列
    std::vector<ColliderComponent*> pending_remove_; //解除用の待機列
    bool dispatching_ = false; //コールバック呼び出しFlag
};

static bool IntersectMeshSphere(const MeshColliderComponent* mesh,const Sphere& sphere);
static bool IntersectMeshBox(const MeshColliderComponent* mesh,const Box& box);
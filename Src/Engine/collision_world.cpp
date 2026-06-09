#include "collision_world.h"

#include <algorithm>
#include <cassert>

#include "../Core/Math/intersect.h"
#include "Components/collider_component.h"
#include "Components/sphere_collider_componet.h"

void CollisionWorld::Collect()
{
    std::set<Pair> cur_pairs; //今のフレーム重なってるペア
    for (int i = 0; i < colliders_.size(); ++i)
    {
        for (int j = i + 1; j < colliders_.size(); ++j)
        {
            ColliderComponent* coll1 = colliders_[i];
            ColliderComponent* coll2 = colliders_[j];
            //所有者が同じだったらcontinue
            if (coll1->GetOwner() == coll2->GetOwner())
            {
                continue;
            }

            //Transformを持ってなかったらcontinue
            if (!coll1->HasTransform() || !coll2->HasTransform())
            {
                continue;
            }

            if (TestCollision(coll1, coll2))
            {
                cur_pairs.insert(MakePair(coll1, coll2));
            }
        }
    }
    dispatching_ = true;
    for (const Pair& p : cur_pairs)
    {
        // prev に無い
        if (prev_pairs_.find(p) == prev_pairs_.end())
        {
            p.first->InvokeBeginOverlap(p.second);
            p.second->InvokeBeginOverlap(p.first);
        }
    }

    for (const Pair& p : prev_pairs_)
    {
        // cur に無い
        if (cur_pairs.find(p) == cur_pairs.end())
        {
            p.first->InvokeEndOverlap(p.second);
            p.second->InvokeEndOverlap(p.first);
        }
    }
    prev_pairs_ = cur_pairs;
    dispatching_ = false;

    // 通知中に積まれた解除を反映。colliders_ だけでなく prev_pairs_ からも
    // 該当ペアを消し、破棄済みポインタを次フレームへ持ち越さない。
    for (ColliderComponent* coll : pending_remove_)
    {
        std::erase(colliders_, coll);
        std::erase_if(prev_pairs_, [coll](const Pair& p)
        {
            return p.first == coll || p.second == coll;
        });
    }
    pending_remove_.clear();
    for (ColliderComponent* coll : pending_add_)
    {
        colliders_.push_back(coll);
    }
    pending_add_.clear();
}

void CollisionWorld::Register(ColliderComponent* coll)
{
    if (dispatching_)
    {
        pending_add_.push_back(coll);
        return;
    }
    assert(std::find(colliders_.begin(), colliders_.end(), coll) == colliders_.end());
    colliders_.push_back(coll);
}

void CollisionWorld::Unregister(ColliderComponent* coll)
{
    if (dispatching_)
    {
        pending_remove_.push_back(coll);
        return;
    }
    std::erase(colliders_, coll);
}

bool CollisionWorld::TestCollision(ColliderComponent* coll1, ColliderComponent* coll2)
{
    if (coll1->GetColliderShape() == ColliderShape::kSphere
        && coll2->GetColliderShape() == ColliderShape::kSphere)
    {
        auto* c1 = static_cast<SphereColliderComponent*>(coll1);
        auto* c2 = static_cast<SphereColliderComponent*>(coll2);
        Sphere sphere1, sphere2;
        c1->GetColliderShapeData(sphere1);
        c2->GetColliderShapeData(sphere2);
        return Intersect(sphere1, sphere2);
    }
    return false;
}

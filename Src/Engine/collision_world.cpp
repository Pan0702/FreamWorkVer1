#include "collision_world.h"

#include <algorithm>
#include <cassert>

#include "../Core/Math/intersect.h"
#include "Components/box_collider_component.h"
#include "Components/collider_component.h"
#include "Components/mesh_collider_component.h"
#include "Components/sphere_collider_componet.h"

void CollisionWorld::Collect()
{
    std::set<HitPair> cur_pairs; //今のフレーム重なってるペア
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
                cur_pairs.emplace(HitPair(coll1, coll2));
            }
        }
    }
    dispatching_ = true;
    for (const HitPair& p : cur_pairs)
    {
        // prev に無い
        if (prev_pairs_.find(p) == prev_pairs_.end())
        {
            p.collider1->InvokeBeginOverlap(p.collider2);
            p.collider2->InvokeBeginOverlap(p.collider1);
        }
   }

    for (const HitPair& p : prev_pairs_)
    {
        // cur に無い
        if (cur_pairs.find(p) == cur_pairs.end())
        {
            p.collider1->InvokeEndOverlap(p.collider2);
            p.collider2->InvokeEndOverlap(p.collider1);
        }
    }
    prev_pairs_ = cur_pairs;
    dispatching_ = false;

    // 通知中に積まれた解除を反映。colliders_ だけでなく prev_pairs_ からも
    // 該当ペアを消し、破棄済みポインタを次フレームへ持ち越さない。
    for (ColliderComponent* coll : pending_remove_)
    {
        std::erase(colliders_, coll);
        std::erase_if(prev_pairs_, [coll](const HitPair& p)
        {
            return p.collider1 == coll || p.collider2 == coll;
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
        Sphere sphere1 = c1->GetColliderShapeData();
        Sphere sphere2 = c2->GetColliderShapeData();
        return Intersect(sphere1, sphere2);
    }

    if (coll1->GetColliderShape() == ColliderShape::kBox
        && coll2->GetColliderShape() == ColliderShape::kBox)
    {
        auto* c1 = static_cast<BoxColliderComponent*>(coll1);
        auto* c2 = static_cast<BoxColliderComponent*>(coll2);
        Box box1 = c1->GetColliderBoxData();
        Box box2 = c2->GetColliderBoxData();
        return Intersect(box1, box2);
    }
    
    if (coll1->GetColliderShape() == ColliderShape::kSphere
        && coll2->GetColliderShape() == ColliderShape::kBox)
    {
        auto* c1 = static_cast<SphereColliderComponent*>(coll1);
        auto* c2 = static_cast<BoxColliderComponent*>(coll2);
        Sphere sphere = c1->GetColliderShapeData();
        Box box = c2->GetColliderBoxData();
        return Intersect(sphere, box);
    }
    if (coll1->GetColliderShape() == ColliderShape::kBox
        && coll2->GetColliderShape() == ColliderShape::kSphere)
    {
        auto* c1 = static_cast<BoxColliderComponent*>(coll1);
        auto* c2 = static_cast<SphereColliderComponent*>(coll2);
        Box box = c1->GetColliderBoxData();
        Sphere sphere = c2->GetColliderShapeData();
        return Intersect(sphere, box);
    }
    
    if (coll1->GetColliderShape() == ColliderShape::kMesh
    && coll2->GetColliderShape() == ColliderShape::kSphere)
    {
        auto* m  = static_cast<MeshColliderComponent*>(coll1);
        auto* sp = static_cast<SphereColliderComponent*>(coll2);
        return IntersectMeshSphere(m, sp->GetColliderShapeData());
    }
    if (coll1->GetColliderShape() == ColliderShape::kSphere
        && coll2->GetColliderShape() == ColliderShape::kMesh)
    {
        auto* sp = static_cast<SphereColliderComponent*>(coll1);
        auto* m  = static_cast<MeshColliderComponent*>(coll2);
        return IntersectMeshSphere(m, sp->GetColliderShapeData());
    }
    if (coll1->GetColliderShape() == ColliderShape::kMesh
        && coll2->GetColliderShape() == ColliderShape::kBox)
    {
        auto* m1 = static_cast<MeshColliderComponent*>(coll1);
        auto* m2 = static_cast<BoxColliderComponent*>(coll2);
        return IntersectMeshBox(m1, m2->GetColliderBoxData());
    }
    if (coll1->GetColliderShape() == ColliderShape::kBox
    && coll2->GetColliderShape() == ColliderShape::kMesh)
    {
        auto* m1 = static_cast<BoxColliderComponent*>(coll1);
        auto* m2 = static_cast<MeshColliderComponent*>(coll2);
        return IntersectMeshBox(m2, m1->GetColliderBoxData());
    }
    // Mesh×Mesh は意図的に非対応。メッシュコライダーは静的な world ジオメトリ用で、
    // 動的メッシュ同士の判定は重い割に用途が乏しいため、ここには分岐を足さず false を返す。
    // 必要になったら BVH 等の高速化を入れてから対応する。
    return false;
}

static bool IntersectMeshSphere(const MeshColliderComponent* mesh,const Sphere& sphere)
{
    const std::vector<Vec3>& vertices = mesh->GetVertices();
    const std::vector<uint32>& indices = mesh->GetIndices();
    const Mat world = mesh->GetWorldMatrix();
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        if (Intersect(sphere,a,b,c))
        {
            return true;
        }
    }
    return false;
}

static bool IntersectMeshBox(const MeshColliderComponent* mesh, const Box& box)
{
    const std::vector<Vec3>& vertices = mesh->GetVertices();
    const std::vector<uint32>& indices = mesh->GetIndices();
    const Mat world = mesh->GetWorldMatrix();
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        if (Intersect(box,a,b,c))
        {
            return true;
        }
    }
    return false;
}


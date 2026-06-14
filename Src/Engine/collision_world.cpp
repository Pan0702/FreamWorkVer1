#include "collision_world.h"

#include <algorithm>
#include <cassert>
#include <utility>

#include "../Core/Math/intersect.h"
#include "Components/box_collider_component.h"
#include "Components/collider_component.h"
#include "Components/mesh_collider_component.h"
#include "Components/sphere_collider_componet.h"

namespace
{
    // ComputeContact 用の型付きデータ取り出し//
    Sphere SphereData(ColliderComponent* c)
    {
        return static_cast<SphereColliderComponent*>(c)->GetColliderShapeData();
    }

    Box BoxData(ColliderComponent* c)
    {
        return static_cast<BoxColliderComponent*>(c)->GetColliderBoxData();
    }

    const MeshColliderComponent* MeshOf(ColliderComponent* c)
    {
        return static_cast<MeshColliderComponent*>(c);
    }
}

void CollisionWorld::Collect()
{
    dispatching_ = true;
    std::set<HitPair> cur_pairs; //今のフレーム重なってるペア
    for (int i = 0; i < colliders_.size(); ++i)
    {
        for (int j = i + 1; j < colliders_.size(); ++j)
        {
            ColliderComponent* coll1 = colliders_[i];
            ColliderComponent* coll2 = colliders_[j];
            //所有者が同じだったらcontinue //
            if (coll1->GetOwner() == coll2->GetOwner())
            {
                continue;
            }
            ContactInfo info;
            if (ComputeContact(coll1, coll2, info))
            {
                cur_pairs.emplace(HitPair(coll1, coll2)); // Overlap //
                coll1->InvokeHit(coll2, info); // Hit //
                ContactInfo flipped = info;
                flipped.normal = info.normal * -1.0f;
                coll2->InvokeHit(coll1, flipped);
            }
        }
    }

    for (const HitPair& p : cur_pairs)
    {
        // prev に無い //
        if (prev_pairs_.find(p) == prev_pairs_.end())
        {
            p.collider1->InvokeBeginOverlap(p.collider2);
            p.collider2->InvokeBeginOverlap(p.collider1);
        }
    }

    for (const HitPair& p : prev_pairs_)
    {
        // cur に無い //
        if (cur_pairs.find(p) == cur_pairs.end())
        {
            p.collider1->InvokeEndOverlap(p.collider2);
            p.collider2->InvokeEndOverlap(p.collider1);
        }
    }
    prev_pairs_ = cur_pairs;
    dispatching_ = false;

    // 通知中に積まれた解除を反映。colliders_ だけでなく prev_pairs_ からも
    // 該当ペアを消し、破棄済みポインタを次フレームへ持ち越さない。//
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
    // prev_pairs_ に残った破棄済みポインタも消す。次フレームの EndOverlap で触らないように //
    std::erase_if(prev_pairs_, [coll](const HitPair& p)
    {
        return p.collider1 == coll || p.collider2 == coll;
    });
}

bool CollisionWorld::ComputeContact(ColliderComponent* coll1, ColliderComponent* coll2, ContactInfo& out)
{
    // 規約: out.normal は coll1 を coll2 から押し出す向き。
    // 形状を kSphere < kBox < kMesh の正準順に並べ替え、逆順だったら最後に
    // 法線を反転する。正準順なら各 Contact の第1引数が coll1 に一致するので、
    // 分岐内での反転は要らない。//
    ColliderShape s1 = coll1->GetColliderShape();
    ColliderShape s2 = coll2->GetColliderShape();

    bool flipped = false;
    if (s1 > s2)
    {
        std::swap(coll1, coll2);
        std::swap(s1, s2);
        flipped = true;
    }

    bool hit = false;
    if (s1 == ColliderShape::kSphere && s2 == ColliderShape::kSphere)
    {
        hit = Contact(SphereData(coll1), SphereData(coll2), out);
    }
    else if (s1 == ColliderShape::kSphere && s2 == ColliderShape::kBox)
    {
        hit = Contact(SphereData(coll1), BoxData(coll2), out);
    }
    else if (s1 == ColliderShape::kSphere && s2 == ColliderShape::kMesh)
    {
        hit = ContactMeshSphere(MeshOf(coll2), SphereData(coll1), out);
    }
    else if (s1 == ColliderShape::kBox && s2 == ColliderShape::kBox)
    {
        hit = Contact(BoxData(coll1), BoxData(coll2), out);
    }
    else if (s1 == ColliderShape::kBox && s2 == ColliderShape::kMesh)
    {
        hit = ContactMeshBox(MeshOf(coll2), BoxData(coll1), out);
    }
    // kMesh × kMeshは重たいので書かない//

    if (hit && flipped)
    {
        out.normal = out.normal * -1.0f;
    }
    return hit;
}

void CollisionWorld::DrawDebug()
{
    for (ColliderComponent* coll : colliders_)
    {
        if (coll->IsDraw())
        {
            debug_objects_.push_back(coll);
        }
    }

    for (ColliderComponent* coll : debug_objects_)
    {
        coll->DrawDebug();
    }
}

static bool ContactMeshBox(const MeshColliderComponent* mesh, const Box& box, ContactInfo& out)
{
    const std::vector<Vec3>& vertices = mesh->GetVertices();
    const std::vector<uint32>& indices = mesh->GetIndices();
    const Mat world = mesh->GetWorldMatrix();
    bool hit = false;
    float best_depth = 0.0f;
    ContactInfo contact;
    for (int i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        if (Contact(box, a, b, c, contact))
        {
            if (contact.depth > best_depth)
            {
                best_depth = contact.depth;
                hit = true;
                out = contact;
            }
        }
    }
    return hit;
}

bool ContactMeshSphere(const MeshColliderComponent* mesh, const Sphere& sphere1, ContactInfo& out)
{
    const std::vector<Vec3>& vertices = mesh->GetVertices();
    const std::vector<uint32>& indices = mesh->GetIndices();
    const Mat world = mesh->GetWorldMatrix();
    bool hit = false;
    float best_depth = 0.0f;
    ContactInfo contact;
    for (int i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        if (Contact(sphere1, a, b, c, contact))
        {
            if (contact.depth > best_depth)
            {
                best_depth = contact.depth;
                hit = true;
                out = contact;
            }
        }
    }
    return hit;
}

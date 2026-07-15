#include "collision_world.h"

#include <algorithm>
#include <cassert>
#include <utility>

#include "../Core/Math/intersect.h"
#include "Components/box_collider_component.h"
#include "Components/capsule_collider_comp.h"
#include "Components/collider_component.h"
#include "Components/mesh_collider_component.h"
#include "Components/sphere_collider_componet.h"

namespace
{
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

    Capsule CapsuleData(ColliderComponent* c)
    {
        return static_cast<CapsuleColliderComponent*>(c)->GetColliderCapsuleData();
    }

    constexpr int kVertsPerTriangle = 3;
}

void CollisionWorld::Collect()
{
    // このフレームの衝突ペアを収集し、必要な接触通知を発行する。
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
            // 当たってるか
            if (ComputeContact(coll1, coll2, info))
            {
                // Overlap //
                cur_pairs.emplace(HitPair(coll1, coll2)); 
                if (!coll1->IsTrigger() && !coll2->IsTrigger())
                {
                    // Hit //
                    coll1->InvokeHit(coll2, info); 
                    ContactInfo flipped = info;
                    //めり込んだ量を反転　//
                    flipped.normal = info.normal * -1.0f;
                    coll2->InvokeHit(coll1, flipped);
                }
            }
        }
    }

    //通知中にColliderが追加/削除さる可能性があるから
    //ここではcolliders_を変更しない
    //新しく発生した接触だけ通知//
    for (const HitPair& p : cur_pairs)
    {
        // prev に無い //
        if (!prev_pairs_.contains(p))
        {
            p.collider1->InvokeBeginOverlap(p.collider2);
            p.collider2->InvokeBeginOverlap(p.collider1);
        }
    }

    //前フレーム存在して現在のフレームで消えた接触だけ通知
    for (const HitPair& p : prev_pairs_)
    {
        // cur に無い //
        if (!cur_pairs.contains(p))
        {
            p.collider1->InvokeEndOverlap(p.collider2);
            p.collider2->InvokeEndOverlap(p.collider1);
        }
    }
    // 次フレームの差分判定用に現在の接触ペアを保存する。
    prev_pairs_ = cur_pairs;
    dispatching_ = false;
    
    // 通知中に要求された解除をここでまとめて反映する。
    // prev_pairs_ からも消して、解除済み Collider を含むペアを次フレームへ持ち越さない。
    for (ColliderComponent* coll : pending_remove_)
    {
        std::erase(colliders_, coll);
        std::erase_if(prev_pairs_, [coll](const HitPair& p)
        {
            return p.collider1 == coll || p.collider2 == coll;
        });
    }
    
    pending_remove_.clear();
    // 通知中に要求された追加をここでまとめて反映する。
    // 追加 Collider は次フレームの衝突判定から参加する。
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
    // 形状を kSphere < kBox < kCapsule < kMesh の正準順に並べ替え、逆順だったら最後に
    // 法線を反転する。正準順なら各 Contact の第1引数が coll1 に一致するので、
    // 分岐内での反転は要らない。//
    // 形状の組み合わせを正準順へそろえ、対応する接触判定関数を選択する。
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
    else if (s1 == ColliderShape::kCapsule && s2 == ColliderShape::kMesh)
    {
        hit = ContactMeshCapsule(MeshOf(coll2), CapsuleData(coll1), out);
    }
    else if (s1 == ColliderShape::kBox && s2 == ColliderShape::kCapsule)
    {
        hit = ContactBoxCapsule(BoxData(coll1), CapsuleData(coll2), out);
    }
    // kMesh × kMeshは重たいので書かない//

    if (hit && flipped)
    {
        out.normal = out.normal * -1.0f;
    }
    return hit;
}

bool CollisionWorld::Raycast(const Ray& ray, ContactInfo& out, const ColliderComponent* ignore) const
{
    bool hit = false;
    float best_t = ray.distance; // これより近いヒットだけ採用。初期値は最大距離。//
    for (ColliderComponent* coll : colliders_)
    {
        if (coll == ignore)
        {
            continue;
        }
        // 今は地面メッシュのみ相手にする。球/箱レイは後日。//
        if (coll->GetColliderShape() != ColliderShape::kMesh)
        {
            continue;
        }
        const MeshColliderComponent* mesh = MeshOf(coll);
        const std::vector<Vec3>& vertices = mesh->GetVertices();
        const std::vector<uint32>& indices = mesh->GetIndices();
        const Mat world = mesh->GetWorldMatrix();
        for (int i = 0; i + 2 < indices.size(); i += kVertsPerTriangle)
        {
            const Vec3 a = TransformPoint(world, vertices[indices[i]]);
            const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
            const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
            ContactInfo info;
            if (Contact(ray, a, b, c, info))
            {
                // info.depth はレイの距離 t。一番近い三角形を採用。
                if (info.depth < best_t)
                {
                    best_t = info.depth;
                    out = info;
                    hit = true;
                }
            }
        }
    }
    return hit;
}

void CollisionWorld::DrawDebug()
{
    debug_objects_.clear();

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
    //　１つの三角形を１ループで
    for (int i = 0; i + 2 < indices.size(); i += kVertsPerTriangle)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        if (Contact(box, a, b, c, contact))
        {
            // info.depth はレイの距離 t。一番近い三角形を採用。
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
    //　１つの三角形を１ループで
    for (int i = 0; i + 2 < indices.size(); i += kVertsPerTriangle)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        if (Contact(sphere1, a, b, c, contact))
        {
            // info.depth はレイの距離 t。一番近い三角形を採用。
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

bool ContactMeshCapsule(const MeshColliderComponent* mesh, const Capsule& capsule, ContactInfo& out)
{
    const std::vector<Vec3>& vertices = mesh->GetVertices();
    const std::vector<uint32>& indices = mesh->GetIndices();
    const Mat world = mesh->GetWorldMatrix();
    bool hit = false;
    float best_depth = 0.0f;
    ContactInfo contact;
    //　１つの三角形を１ループで
    for (int i = 0; i + 2 < indices.size(); i += kVertsPerTriangle)
    {
        const Vec3 a = TransformPoint(world, vertices[indices[i]]);
        const Vec3 b = TransformPoint(world, vertices[indices[i + 1]]);
        const Vec3 c = TransformPoint(world, vertices[indices[i + 2]]);
        // 
        if (Contact(capsule, a, b, c, contact))
        {
            // info.depth はレイの距離 t。一番近い三角形を採用。
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

bool ContactBoxCapsule(const Box& b, const Capsule& c, ContactInfo& out)
{
    const Vec3 a = c.center - c.dir * c.height;
    const Vec3 e = c.center + c.dir * c.height;

    // 箱の中心に最も近い芯線上の点を求める //
    const Vec3 box_center = (b.min + b.max) * kHalfSize;
    const Vec3 ab = e - a;
    const float len2 = Dot(ab, ab);
    float t = 0.0f;
    if (len2 > kEpsilon) 
    {
        t = Dot(box_center - a, ab) / len2;
        t = std::clamp(t, 0.0f, 1.0f);
    }
    const Vec3 p = a + ab * t;

    // その点に半径 r の球を置いて、既存の sphere×box を流用//
    const Sphere s{p, c.radius};
    if (!Contact(s, b, out))
    {
        return false;
    }
    return true;
}

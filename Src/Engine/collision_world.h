#pragma once
#include <functional>
#include <set>
#include <vector>

#include "../Core/Math/intersect.h"
#include "Components/collider_component.h"
#include "Components/mesh_collider_component.h"

struct Box;
class MeshColliderComponent;
struct Sphere;
struct Ray;

// 接触通知済みのコライダーペアを順序付きで保持するキー。//
struct HitPair
{
    ColliderComponent* collider1;
    ColliderComponent* collider2;

    /**
     * @brief 2 つのコライダーを常に同じ順序で保持する。
     * @param a ペアに含める 1 つ目のコライダー。
     * @param b ペアに含める 2 つ目のコライダー。
     */
    HitPair(ColliderComponent* a, ColliderComponent* b)
    {
        // 生ポインタの < は未規定。必ず std::less を通す。//
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

    /**
     * @brief std::set に保存するため、コライダーペアを安定した順序で比較する。
     * @param o 比較相手の接触ペア。
     * @return 自分のペアが o より前に並ぶ場合は true。
     */
    bool operator<(const HitPair& o) const
    {
        if (collider1 != o.collider1)
            return std::less<ColliderComponent*>{}(collider1, o.collider1);
        return std::less<ColliderComponent*>{}(collider2, o.collider2);
    }
};

// 登録されたコライダーの接触判定、通知、デバッグ描画をまとめる。//
class CollisionWorld
{
public:
    /**
     * @brief 追加・削除待ちのコライダーを反映し、判定対象リストを更新する。
     */
    void Collect();
    /**
     * @brief コライダーを接触判定の管理対象へ登録する。
     * @param coll 登録するコライダー。
     */
    void Register(ColliderComponent* coll);
    /**
     * @brief コライダーを接触判定の管理対象から外す。
     * @param coll 登録解除するコライダー。
     */
    void Unregister(ColliderComponent* coll);
    /**
     * @brief 2 つのコライダー形状に合わせた接触判定を呼び出す。
     * @param coll1 押し出し対象になるコライダー。
     * @param coll2 判定相手になるコライダー。
     * @param out 接触法線と貫通量を書き込む情報。
     * @return coll1 と coll2 が接触している場合は true。
     */
    bool ComputeContact(ColliderComponent* coll1, ColliderComponent* coll2, ContactInfo& out);
    /**
     * @brief 登録済みコライダーに対してレイキャストを行う。
     * @param ray 判定に使用するレイ。
     * @param out ヒット距離と法線を書き込む情報。
     * @param ignore 判定対象から除外するコライダー。
     * @return レイがいずれかのコライダーに当たった場合は true。
     */
    bool Raycast(const Ray& ray, ContactInfo& out, const ColliderComponent* ignore = nullptr) const;
    /**
     * @brief 登録済みコライダーのデバッグ描画を積む。
     */
    void DrawDebug();
    
private:
    std::vector<ColliderComponent*> colliders_; // Collider の配列。//
    std::set<HitPair> prev_pairs_; // 前フレームに重なっていたペア。//
    std::vector<ColliderComponent*> pending_add_; // 登録用の待機列。//
    std::vector<ColliderComponent*> pending_remove_; // 解除用の待機列。//
    std::vector<ColliderComponent*> debug_objects_;
    bool dispatching_ = false; // コールバック呼び出し中かどうか。//
};

/**
 * @brief メッシュと球の当たり判定を行う。
 * @param mesh 判定相手になるメッシュコライダー。
 * @param sphere1 押し出し対象になる球。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return メッシュ内のいずれかの三角形と球が接触した場合は true。
 */
static bool ContactMeshSphere(const MeshColliderComponent* mesh, const Sphere& sphere1, ContactInfo& out);
/**
 * @brief メッシュと AABB の当たり判定を行う。
 * @param mesh 判定相手になるメッシュコライダー。
 * @param box 押し出し対象になる AABB。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return メッシュ内のいずれかの三角形と AABB が接触した場合は true。
 */
static bool ContactMeshBox(const MeshColliderComponent* mesh, const Box& box, ContactInfo& out);

/**
 * @brief メッシュとカプセルの当たり判定を行う。
 * @param mesh 判定相手になるメッシュコライダー。
 * @param capsule 押し出し対象になるカプセル。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return メッシュ内のいずれかの三角形とカプセルが接触した場合は true。
 */
static bool ContactMeshCapsule(const MeshColliderComponent* mesh,const Capsule& capsule, ContactInfo& out);

/**
* @brief ボックスとカプセルの当たり判定を行う。
* @param b 判定相手になるボックス。
* @param c 押し出し対象になるカプセル。
* @param out 接触法線と貫通量を書き込む情報。
* @return メッシュ内のいずれかの三角形とカプセルが接触した場合は true。
*/
static bool ContactBoxCapsule(const Box& b,const Capsule& c, ContactInfo& out);
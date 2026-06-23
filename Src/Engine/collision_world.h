#pragma once
#include <functional>
#include <set>
#include <vector>

#include "Components/collider_component.h"
#include "Components/mesh_collider_component.h"

struct Box;
class MeshColliderComponent;
struct Sphere;
struct Ray;

// HitPair に関係する状態と振る舞いをまとめる型。
struct HitPair
{
    ColliderComponent* collider1;
    ColliderComponent* collider2;

    /**
     * @brief インスタンスの初期状態を整える。
     * @param a 計算に使用するベクトルまたは点。
     * @param b 計算に使用するベクトルまたは点。
     */
    HitPair(ColliderComponent* a, ColliderComponent* b)
    {
        // 生ポインタの < は未規定。必ず std::less を通す//
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
     * @brief 演算子 operator< で値を扱う。
     * @param o o に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    bool operator<(const HitPair& o) const
    {
        if (collider1 != o.collider1)
            return std::less<ColliderComponent*>{}(collider1, o.collider1);
        return std::less<ColliderComponent*>{}(collider2, o.collider2);
    }
};

// CollisionWorld に関係する状態と振る舞いをまとめる型。
class CollisionWorld
{
public:
    /**
     * @brief 登録済みの対象から今回処理する要素を集める。
     */
    void Collect();
    /**
     * @brief 対象を管理リストへ登録する。
     * @param coll 判定または通知に使用するコライダー。
     */
    void Register(ColliderComponent* coll);
    /**
     * @brief 対象を管理リストから外す。
     * @param coll 判定または通知に使用するコライダー。
     */
    void Unregister(ColliderComponent* coll);
    /**
     * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
     * @param coll1 判定または通知に使用するコライダー。
     * @param coll2 判定または通知に使用するコライダー。
     * @param out 計算結果を書き込む情報。
     * @return 接触点と法線を計算できた場合は true。
     */
    bool ComputeContact(ColliderComponent* coll1, ColliderComponent* coll2, ContactInfo& out);
    /**
     * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
     * @param ray 判定に使用するレイ。
     * @param out 計算結果を書き込む情報。
     * @param ignore 判定対象から除外するコライダー。
     * @return レイがコライダーに当たり、ヒット情報を書き込めた場合は true。
     */
    bool Raycast(const Ray& ray, ContactInfo& out, const ColliderComponent* ignore = nullptr);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void DrawDebug();
    
private:
    std::vector<ColliderComponent*> colliders_; //Colliderの配列 //
    std::set<HitPair> prev_pairs_; //前フレーム重なってたペア //
    std::vector<ColliderComponent*> pending_add_; //登録用の待機列 //
    std::vector<ColliderComponent*> pending_remove_; //解除用の待機列 //
    std::vector<ColliderComponent*> debug_objects_;
    bool dispatching_ = false; //コールバック呼び出しFlag//
};

/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param mesh 読み込み、描画、または判定に使用するメッシュ。
 * @param sphere1 sphere1 に設定する値。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
static bool ContactMeshSphere(const MeshColliderComponent* mesh, const Sphere& sphere1, ContactInfo& out);
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param mesh 読み込み、描画、または判定に使用するメッシュ。
 * @param box box に設定する値。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
static bool ContactMeshBox(const MeshColliderComponent* mesh, const Box& box, ContactInfo& out);

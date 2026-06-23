#pragma once
#include <cstdint>
#include <functional>
#include "../component.h"
#include "../../Core/Math/my_math.h"
#include "../../Debug/debug.h"
struct ContactInfo;
class CollisionWorld;
struct AttachContext;

enum class ColliderShape : uint8_t
{
    kSphere,
    kBox,
    kMesh,
};

// Actor に追加して使う ColliderComponent の状態と処理をまとめる。
class ColliderComponent : public Component
{
public:
    using OverlapCallback =
    std::function<void(const ColliderComponent* self, const ColliderComponent* other)>;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param callback 通知時に呼び出すコールバック。
     */
    void SetOnBeginOverlap(OverlapCallback callback);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param callback 通知時に呼び出すコールバック。
     */
    void SetOnEndOverlap(OverlapCallback callback);

    /**
     * @brief 指定された値を内部状態に反映する。
     * @param obj メンバ関数を呼び出す対象オブジェクト。
     */
    template <class T>
    void SetOnBeginOverlap(T* obj, void (T::*fn)(const ColliderComponent*, const ColliderComponent*))
    {
        on_begin_ = [obj, fn](const ColliderComponent* s, const
                              ColliderComponent* o)
        {
            (obj->*fn)(s, o);
        };
    }

    /**
     * @brief 指定された値を内部状態に反映する。
     * @param obj メンバ関数を呼び出す対象オブジェクト。
     */
    template <class T>
    void SetOnEndOverlap(T* obj, void (T::*fn)(const ColliderComponent*, const ColliderComponent*))
    {
        on_end_ = [obj, fn](const ColliderComponent* s, const
                            ColliderComponent* o)
        {
            (obj->*fn)(s, o);
        };
    }

    using HitCallback =
    std::function<void(ColliderComponent* self, Actor* other_actor, ColliderComponent* other_coll,
                       const ContactInfo& info)>;

    /**
     * @brief 指定された値を内部状態に反映する。
     * @param obj メンバ関数を呼び出す対象オブジェクト。
     */
    template <class T>
    void SetOnHit(T* obj, void (T::*fn)(ColliderComponent*, Actor*, ColliderComponent*, const ContactInfo&))
    {
        on_hit_ = [obj,fn](ColliderComponent* self, Actor* other_actor, ColliderComponent* oc, const ContactInfo& info)
        {
            (obj->*fn)(self, other_actor, oc, info);
        };
    }

    /**
     * @brief コライダーの形状種別を取得する。
     * @return 現在保持している コライダーの形状種別。
     */
    virtual ColliderShape GetColliderShape() const = 0;
    /**
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 共有コンテキストから登録を外し、システム側の参照を切る。
     */
    void OnDetach() override;
    /**
     * @brief コライダー判定に使う中心とスケールを取得する。
     * @param center center に設定する値。
     * @param abs_scale abs_scale に設定する値。
     * @return コライダー判定に使う中心とスケールを取得できた 場合は true。
     */
    bool TryGetColliderTransform(Vec3* center, Vec3* abs_scale) const;
    /**
     * @brief 重なり始めた通知を登録済みコールバックへ流す。
     * @param other other に設定する値。
     */
    void InvokeBeginOverlap(const ColliderComponent* other) const;
    /**
     * @brief 重なりが終わった通知を登録済みコールバックへ流す。
     * @param other other に設定する値。
     */
    void InvokeEndOverlap(const ColliderComponent* other);
    /**
     * @brief 接触中の通知を登録済みコールバックへ流す。
     * @param other other に設定する値。
     * @param info 計算結果を書き込む情報。
     */
    void InvokeHit(ColliderComponent* other, const ContactInfo& info);

    /**
     * @brief 指定された値を内部状態に反映する。
     * @param color 設定する色。
     */
    void SetColor(const Vec4& color);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param draw デバッグ描画を有効にするかどうか。
     */
    void SetDraw(bool draw);
    /**
     * @brief 現在の状態が条件を満たしているか調べる。
     * @return デバッグ描画が有効な場合は true。
     */
    bool IsDraw() const;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param use_transform Owner の Transform を判定へ反映するかどうか。
     */
    void SetUseTransform(bool use_transform);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    virtual void DrawDebug() const = 0;
    
protected:
    Vec4 color_ = Vec4(1, 0, 1, 0);
    bool is_draw_ = false;
    bool use_transform_ = true;
private:

    OverlapCallback on_begin_;
    OverlapCallback on_end_;
    HitCallback on_hit_;
    CollisionWorld* collision_world_ = nullptr;
};

/**
 * @brief Owner の Transform を判定へ反映するかどうかを設定する。
 * @param use_transform Owner の Transform を判定へ反映するかどうか。
 */
inline void ColliderComponent::SetUseTransform(bool use_transform)
{
    use_transform_ = use_transform;
}

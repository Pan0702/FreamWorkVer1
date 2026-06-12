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

class ColliderComponent : public Component
{
public:
    using OverlapCallback =
    std::function<void(const ColliderComponent* self, const ColliderComponent* other)>;
    void SetOnBeginOverlap(OverlapCallback callback);
    void SetOnEndOverlap(OverlapCallback callback);

    template <class T>
    void SetOnBeginOverlap(T* obj, void (T::*fn)(const ColliderComponent*, const ColliderComponent*))
    {
        on_begin_ = [obj, fn](const ColliderComponent* s, const
                              ColliderComponent* o)
        {
            (obj->*fn)(s, o);
        };
    }

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

    template <class T>
    void SetOnHit(T* obj, void (T::*fn)(ColliderComponent*, Actor*, ColliderComponent*, const ContactInfo&))
    {
        on_hit_ = [obj,fn](ColliderComponent* self, Actor* other_actor, ColliderComponent* oc, const ContactInfo& info)
        {
            (obj->*fn)(self, other_actor, oc, info);
        };
    }

    virtual ColliderShape GetColliderShape() const = 0;
    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    bool TryGetColliderTransform(Vec3* center, Vec3* abs_scale) const;
    void InvokeBeginOverlap(const ColliderComponent* other) const;
    void InvokeEndOverlap(const ColliderComponent* other);
    void InvokeHit(ColliderComponent* other, const ContactInfo& info);

    void SetColor(const Vec4& color);
    void SetDraw(bool draw);
    bool IsDraw() const;
    virtual void DrawDebug() const = 0;
    
protected:
    Vec4 color_ = Vec4(1, 1, 1, 1);
    bool is_draw_ = false;
    
private:

    OverlapCallback on_begin_;
    OverlapCallback on_end_;
    HitCallback on_hit_;
    CollisionWorld* collision_world_ = nullptr;
};

#pragma once
#include <cstdint>
#include <functional>
#include "../component.h"
#include "../../Core/Math/my_math.h"
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
    virtual ColliderShape GetColliderShape() const = 0;
    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    bool TryGetColliderTransform(Vec3* center, Vec3* abs_scale) const;
    bool HasTransform() const;
    void InvokeBeginOverlap(const ColliderComponent* other) const;
    void InvokeEndOverlap(const ColliderComponent* other);

private:
    OverlapCallback on_begin_;
    OverlapCallback on_end_;
    CollisionWorld* collision_world_ = nullptr;
    bool has_transform_ = false;
};

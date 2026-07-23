#pragma once
#include "../component.h"
#include "../../Core/Math/my_math.h"
class NavigationSystem;
class MeshColliderComponent;
struct NavigationGeometry;

class NavigationSourceComponent : public Component
{
public:
    explicit NavigationSourceComponent(MeshColliderComponent* mesh);
    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    NavigationGeometry GetGeometry() const;

private:
    MeshColliderComponent* mesh_;
    NavigationSystem* navigation_system_ = nullptr;
    uint32 source_id_                    = 0;
};

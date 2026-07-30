#pragma once
#include <vector>

#include "navigation_config.h"
#include "../../Core/Math/my_math.h"
#include "navigation_geometry.h"
#include "navigation_type.h"

class NavigationSourceComponent;

class NavigationSystem
{
public:
    uint32 RegisterSource(NavigationSourceComponent* component);
    void UnregisterSource(uint32 source_id);
    std::vector<NavigationGeometry> CollectGeometries() const;
    bool Rebuild(const NavigationConfig& config);
    bool FindPath(const Vec3& start_position, const Vec3& goal_position,
                  std::vector<Vec3>& out_path) const;

    const NavigationMeshData& GetMeshData() const;
    void DrawDebug() const;
private:
    struct RegisteredSource
    {
        NavigationSourceComponent* component = nullptr;
        uint32 id = 0;
    };
    std::vector<RegisteredSource> sources_;
    uint32 next_id_ = 1;
    NavigationMeshData mesh_data_;
};

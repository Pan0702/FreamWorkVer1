#pragma once
#include <vector>

#include "../../Core/Math/my_math.h"
#include "navigation_geometry.h"

class NavigationSourceComponent;

class NavigationSystem
{
public:
    uint32 RegisterSource(NavigationSourceComponent* component);
    void UnregisterSource(uint32 source_id);
    std::vector<NavigationGeometry> CollectGeometries() const;
private:
    struct RegisteredSource
    {
        NavigationSourceComponent* component = nullptr;
        uint32 id = 0;
    };
    std::vector<RegisteredSource> sources_;
    uint32 next_id_ = 1;
};

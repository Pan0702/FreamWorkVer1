#pragma once
#include "../Engine/Navigation/navigation_type.h"

class NavigationDebugRenderer
{
public:
    void Draw(const NavigationMeshData& mesh_data, float height_offset = 0.2f) const;
};

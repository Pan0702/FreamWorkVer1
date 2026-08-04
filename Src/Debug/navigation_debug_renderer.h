#pragma once
#include "../Engine/Navigation/navigation_type.h"

class NavigationDebugRenderer
{
public:
    void Draw(const NavigationMeshData& mesh_data, float height_offset = 0.2f) const;
    void DrawCorridor(const NavigationMeshData& mesh_data,
                      const std::vector<uint32_t>& polygon_indices,
                      float height_offset = 1.0f) const;
    void Draw(const NavigationDetailMeshData& detail_mesh_data, float height_offset = 0.2f) const;
};

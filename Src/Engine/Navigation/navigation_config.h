#pragma once
#include <cstdint>

struct NavigationConfig
{
    float cell_size = 20.f;
    float cell_height = 10.f;
  
    float agent_height = 180.0f;
    float agent_radius = 40.0f;
    float agent_max_climb = 40.0f;
    float agent_max_slope_deg = 45.0f;
    float max_contour_simplification_error  = 20.0;
    uint32_t min_region_span_count = 64;
    uint32_t merge_region_span_count = 400;
    uint32_t max_vertex_per_poly = 6;
};

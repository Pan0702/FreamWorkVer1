#pragma once
#include <cstdint>

struct NavigationSpan
{
    uint32_t min_height = 0;
    uint32_t max_height = 0;
    bool is_walk = false;
};

struct NavigationCompactSpan
{
    uint32_t floor_height = 0;
    uint32_t clearance_height = 0;
    uint32_t dis_to_wall = UINT32_MAX;
    uint32_t region_id = 0;
    uint32_t connection_indices[4] = {
        UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX
    };
    bool is_walk = true ;
    
};

struct NavigationCompactCell
{
    uint32_t first_span_index = 0;
    uint32_t span_count = 0;
};

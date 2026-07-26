#include "navigation_mesh_builder.h"
#include <algorithm>
#include "navigation_config.h"
#include "navigation_geometry.h"
#include "navigation_heightfield.h"

Box NavigationMeshBuilder::CalcCellBounds(const NavigationHeightfield& height, uint32 x, uint32 z) const
{
    Box b;
    b.min.x = height.GetWorldBounds().min.x + x * height.GetCellSize();
    b.max.x = (std::min)(b.min.x + height.GetCellSize(), height.GetWorldBounds().max.x);

    b.min.y = height.GetWorldBounds().min.y;
    b.max.y = height.GetWorldBounds().max.y;

    b.min.z = height.GetWorldBounds().min.z + z * height.GetCellSize();
    b.max.z = (std::min)(b.min.z + height.GetCellSize(), height.GetWorldBounds().max.z);
    return b;
}

std::vector<Vec3> NavigationMeshBuilder::ClipTriangleToCell(const Triangle& tri, const Box& b) const
{
    std::vector<Vec3> result = {tri.a, tri.b, tri.c};
    result = ClipPolygonAgainstMinX(result, b.min.x);
    result = ClipPolygonAgainstMaxX(result, b.max.x);
    result = ClipPolygonAgainstMinZ(result, b.min.z);
    result = ClipPolygonAgainstMaxZ(result, b.max.z);
    return result;
}

bool NavigationMeshBuilder::Build(const std::vector<NavigationGeometry>& geometries,const NavigationConfig& config,
    NavigationHeightfield& heightfield)
{
    bool success_flag = false;
    for (auto& geometry : geometries)
    {
        success_flag |= RasterizeGeometry(geometry, config, heightfield);
    }
    heightfield.MergeSpans();
    FilterLowCeilingSpans(heightfield, config);
    return success_flag;
}

bool NavigationMeshBuilder::RasterizeGeometry(const NavigationGeometry& geometry, const NavigationConfig& config,
                                              NavigationHeightfield& height)const
{
    bool success_flag = false;
    for (uint32 i = 0; i < geometry.indices.size(); i += 3)
    {
        const Triangle tri = GetWorldTriangle(geometry, i);
        success_flag |= RasterizeTriangle(tri, config, height);
    }
    return success_flag;
}

bool NavigationMeshBuilder::RasterizeTriangle(const Triangle& tri, const NavigationConfig& config,
                                              NavigationHeightfield& height)const 
{
    Box b = CalcTriangleBounds(tri);
    CellRange range;
    if (!CalcCellRange(b, height, range))
    {
        return false;
    }
    const bool is_walk = IsWalkableTriangle(tri, config);
     bool is_push_back = false;
    for (int32 z = range.min_depth_cell; z <= range.max_depth_cell; ++z)
    {
        for (int32 x = range.min_width_cell; x <= range.max_width_cell; ++x)
        {
            const Box cell_b = CalcCellBounds(height, x, z);
            const std::vector<Vec3> clipped_vertices = ClipTriangleToCell(tri, cell_b);
            if (clipped_vertices.empty())
            {
                continue;
            }
            float min_y, max_y;
            if (!CalcPolygonHeightRange(clipped_vertices, min_y, max_y))
            {
                continue;
            }
            NavigationSpan span;
            if (!CreateSpanFromHeightRange(min_y, max_y, height, is_walk, span))
            {
                continue;
            }
            
            if (height.AddSpan(x,z,span))
            {
                is_push_back = true;
            }
        }
    }
    return is_push_back;
}

void NavigationMeshBuilder::FilterLedgeSpans(NavigationHeightfield& heightfield, const NavigationConfig& config)
{
    for (int x = 0; x < heightfield.GetWidth(); ++x)
    {
        for (int z = 0; z < heightfield.GetDepth(); ++z)
        {
            auto cell = heightfield.GetCell(x,z);
            for (int c = 0; c + 1 < cell->spans.size(); ++c)
            {
                if (!cell->spans[c].is_walk)
                {
                    continue;
                }
                
            }
        }
    }
}

void NavigationMeshBuilder::FilterLowCeilingSpans(NavigationHeightfield& heightfield, const NavigationConfig& config)
{
    for (int x = 0; x < heightfield.GetWidth(); ++x)
    {
        for (int z = 0; z < heightfield.GetDepth(); ++z)
        {
            auto cell = heightfield.GetCell(x,z);
            for (int c = 0; c + 1 < cell->spans.size(); ++c)
            {
                auto& span = cell->spans[c];
                auto& next_span = cell->spans[c + 1];
                const float height = (next_span.min_height - span.max_height) * heightfield.GetCellHeight();
                if (height < config.agent_height)
                {
                    span.is_walk = false;
                }
            }
        }
    }
}

bool NavigationMeshBuilder::CreateSpanFromHeightRange(float min_y, float max_y,
                                                      const NavigationHeightfield& height, bool is_walk,
                                                      NavigationSpan& span) const
{
    //低い床よりもMaxが高かったらfalse
    if (min_y > max_y)
    {
        return false;
    }
    //セルにスペースがなかったらfalse
    if (height.GetCellHeight() <= 0.0f)
    {
        return false;
    }
    //Yが範囲外ならfalse
    if (min_y > height.GetWorldBounds().max.y || max_y < height.GetWorldBounds().min.y)
    {
        return false;
    }

    float clamped_min_y = std::clamp(min_y, height.GetWorldBounds().min.y,
                                     height.GetWorldBounds().max.y);
    float clamped_max_y = std::clamp(max_y, height.GetWorldBounds().min.y,
                                     height.GetWorldBounds().max.y);
    float relative_min = (clamped_min_y - height.GetWorldBounds().min.y)
        / height.GetCellHeight();
    float relative_max = (clamped_max_y - height.GetWorldBounds().min.y)
        / height.GetCellHeight();

    span.min_height = static_cast<uint32>(std::floor(relative_min));
    span.max_height = static_cast<uint32>(std::ceil(relative_max));

    span.is_walk = is_walk;
    return true;
}

bool NavigationMeshBuilder::IsWalkableTriangle(const Triangle& tri, const NavigationConfig& config) const
{
    const Vec3 side_1 = tri.b - tri.a;
    const Vec3 side_2 = tri.c - tri.a;
    Vec3 normal = Cross(side_1, side_2);
    if (normal.LengthSquared() < kEpsilon)
    {
        return false;
    }
    normal.Normalize();
    const Vec3 kUp(0, 1, 0);
    float up_dot = Dot(normal, kUp);
    return up_dot >= std::cos(config.agent_max_slope_deg * kDegToRad);
}

Triangle NavigationMeshBuilder::GetWorldTriangle(const NavigationGeometry& geometry, uint32 begin) const
{
    Triangle result;
    uint32 index_a = geometry.indices[begin];
    uint32 index_b = geometry.indices[begin + 1];
    uint32 index_c = geometry.indices[begin + 2];

    result.a = TransformPoint(geometry.world_mat, geometry.vertices[index_a]);
    result.b = TransformPoint(geometry.world_mat, geometry.vertices[index_b]);
    result.c = TransformPoint(geometry.world_mat, geometry.vertices[index_c]);
    return result;
}

Box NavigationMeshBuilder::CalcTriangleBounds(const Triangle& tri) const
{
    Box result;
    result.max.x = (std::max)(tri.a.x, (std::max)(tri.b.x, tri.c.x));
    result.max.y = (std::max)(tri.a.y, (std::max)(tri.b.y, tri.c.y));
    result.max.z = (std::max)(tri.a.z, (std::max)(tri.b.z, tri.c.z));
    result.min.x = (std::min)(tri.a.x, (std::min)(tri.b.x, tri.c.x));
    result.min.y = (std::min)(tri.a.y, (std::min)(tri.b.y, tri.c.y));
    result.min.z = (std::min)(tri.a.z, (std::min)(tri.b.z, tri.c.z));
    return result;
}

bool NavigationMeshBuilder::CalcCellRange(const Box& b, const NavigationHeightfield& height, CellRange& range) const
{
    if (height.GetWidth() == 0 || height.GetDepth() == 0)
    {
        return false;
    }

    if (b.max.x < height.GetWorldBounds().min.x || b.min.x > height.GetWorldBounds().max.x
        || b.max.z < height.GetWorldBounds().min.z || b.min.z > height.GetWorldBounds().max.z)
    {
        return false;
    }

    int32 min_x = static_cast<int32>((std::floor)((b.min.x - height.GetWorldBounds().min.x) / height.GetCellSize()));
    int32 min_z = static_cast<int32>((std::floor)((b.min.z - height.GetWorldBounds().min.z) / height.GetCellSize()));
    int32 max_x = static_cast<int32>((std::floor)((b.max.x - height.GetWorldBounds().min.x) / height.GetCellSize()));
    int32 max_z = static_cast<int32>((std::floor)((b.max.z - height.GetWorldBounds().min.z) / height.GetCellSize()));
    range.min_width_cell = std::clamp(min_x, 0, static_cast<int32>(height.GetWidth() - 1));
    range.min_depth_cell = std::clamp(min_z, 0, static_cast<int32>(height.GetDepth() - 1));
    range.max_width_cell = std::clamp(max_x, 0, static_cast<int32>(height.GetWidth() - 1));
    range.max_depth_cell = std::clamp(max_z, 0, static_cast<int32>(height.GetDepth() - 1));
    return true;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMinX(const std::vector<Vec3>& vertices, float min_x) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.x >= min_x;
        const bool end_inside = end.x >= min_x;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (min_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (min_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMaxX(const std::vector<Vec3>& vertices, float max_x) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.x <= max_x;
        const bool end_inside = end.x <= max_x;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (max_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (max_x - start.x) / (end.x - start.x);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMinZ(const std::vector<Vec3>& vertices, float min_z) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.z >= min_z;
        const bool end_inside = end.z >= min_z;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (min_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (min_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

std::vector<Vec3> NavigationMeshBuilder::ClipPolygonAgainstMaxZ(const std::vector<Vec3>& vertices, float max_z) const
{
    std::vector<Vec3> result;
    result.clear();
    if (vertices.empty())
    {
        return result;
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        const Vec3 start = vertices[i];
        const Vec3 end = vertices[(i + 1) % vertices.size()];
        const bool start_inside = start.z <= max_z;
        const bool end_inside = end.z <= max_z;
        if (start_inside && end_inside)
        {
            result.push_back(end);
        }
        else if (start_inside)
        {
            const float t = (max_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
        }
        else if (end_inside)
        {
            const float t = (max_z - start.z) / (end.z - start.z);
            result.push_back(start + t * (end - start));
            result.push_back(end);
        }
    }
    return result;
}

bool NavigationMeshBuilder::CalcPolygonHeightRange(const std::vector<Vec3>& vertices,
                                                   float& low_height, float& high_height) const
{
    if (vertices.empty())
    {
        return false;
    }
    float low = FLT_MAX;
    float high = -FLT_MAX;
    for (const Vec3& v : vertices)
    {
        if (v.y < low)
        {
            low = v.y;
        }
        if (v.y > high)
        {
            high = v.y;
        }
    }
    low_height = low;
    high_height = high;
    return true;
}

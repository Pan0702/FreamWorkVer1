#pragma once
#include <vector>

#include "transform_component.h"
#include "../../Core/Math/mat.h"

struct InstanceData
{
    Mat world;
};
class InstancedStaticMeshComponent
{
public:
    InstanceData AddInstance(const TransformComponent& transform);
    void SetInstanceTransform(uint32_t id,const TransformComponent& transform);
    void RemoveInstance(uint32_t id);
    
};

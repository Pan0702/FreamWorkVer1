#pragma once
#include <vector>

#include "transform_component.h"
#include "../../Core/Math/mat.h"
#include "../../Renderer/draw_command.h"

using InstancedID = uint32_t;

class InstancedMeshRenderer;
class MaterialSlot;
class Mesh;

struct InstanceData
{
    InstancedID id;
    Mat local;
};

class InstancedStaticMeshComponent : public Component
{
public:
    InstancedStaticMeshComponent() = default;
    InstancedStaticMeshComponent(Mesh* mesh,MaterialSlot* material);
    ~InstancedStaticMeshComponent() = default;
    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    InstancedID AddInstance(const Mat& transform);
    void SetInstanceTransform(InstancedID id, const Mat& transform);
    void RemoveInstance(InstancedID id);
    void ClearInstances();
    InstanceData* FindInstance(InstancedID id);
    
    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;
    void SetMaterialSlot(MaterialSlot* material);
    MaterialSlot* GetMaterialSlot() const;
    const std::vector<InstanceData>& GetInstances() const;
    
    int sort_key = 0;
private:
    Mesh* mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    InstancedMeshRenderer* renderer_ = nullptr;
    std::vector<InstanceData> instances_;
    
    InstancedID next_id_ = 0;   
};

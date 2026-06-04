#pragma once
#include "../component.h"

class MaterialSlot;
class Material;
class Mesh;
class MeshRenderer;

class StaticMeshComponent : public Component
{
public:
    StaticMeshComponent() = default;
    StaticMeshComponent(Mesh* mesh, MaterialSlot* material_slot);
    ~StaticMeshComponent() override;

    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;
    MaterialSlot* GetMaterialSlot() const;

private:
    Mesh* mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    MeshRenderer* renderer_ = nullptr;
};

#pragma once
#include "../component.h"

class Material;
class Mesh;
class MeshRenderer;

class StaticMeshComponent : public Component
{
public:
    StaticMeshComponent() = default;
    StaticMeshComponent(Mesh* mesh, Material* material);
    ~StaticMeshComponent() override;

    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;

    void SetMesh(Mesh* mesh);
    void SetMaterial(Material* material);
    Mesh* GetMesh() const;
    Material* GetMaterial() const;

private:
    Mesh* mesh_ = nullptr;
    Material* material_ = nullptr;
    MeshRenderer* renderer_ = nullptr;
};

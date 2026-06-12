#pragma once
#include "collider_component.h"
#include <vector>
class Mesh;

class MeshColliderComponent : public ColliderComponent
{
public:
    explicit MeshColliderComponent(const Mesh* mesh = nullptr);
    void SetMesh(const Mesh* mesh = nullptr);
    ColliderShape GetColliderShape() const override;
    void OnAttach(const AttachContext& context) override;
    void SetTriangles(std::vector<Vec3> vertices,std::vector<uint32> indices);
    const std::vector<Vec3>& GetVertices() const;
    const std::vector<uint32>& GetIndices() const;
    Mat GetWorldMatrix() const;

private:
    void DrawDebug() const override;
    std::vector<Vec3> vertices_;
    std::vector<uint32> indices_;
};

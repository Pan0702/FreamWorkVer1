#include "cube.h"

#include "../../Engine/Components/static_mesh_component.h"
#include "../../Resource/mesh.h"
#include "../../Resource/mesh_manager.h"
#include "../../Resource/texture_manager.h"
#include "../../Resource/material_slot.h"

namespace 
{
    const Vec3 kDefaultCubePosition(0, 0, 0);
}

Cube::Cube()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/cube.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = kDefaultCubePosition;
}

Cube::Cube(const Vec3& pos, const Vec3& scale)
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/cube.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = pos + kDefaultCubePosition;
    transform_.scale = scale;
}


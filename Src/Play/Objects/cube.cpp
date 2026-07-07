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
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/Cube.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = kDefaultCubePosition;
}

Cube::Cube(const Vec3& pos, const Vec3& scale)
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/Cube.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = pos + kDefaultCubePosition;
    transform_.scale = scale;
}
Cube::Cube(const std::string& name)
{
    Mesh* mesh = MeshManager::Get().Load(name);
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = kDefaultCubePosition;
    transform_.rotation = Vec3(0, -90.0f * kDegToRad, 0);
}




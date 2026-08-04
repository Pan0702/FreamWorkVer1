#include "slope.h"
#include "../../../Engine/Components/static_mesh_component.h"
#include "../../../Engine/Components/mesh_collider_component.h"
#include "../../../Engine/Components/navigation_source_component.h"

Slope::Slope()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/test_stage.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDesc());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    MeshColliderComponent* mesh_coll = AddComponent<MeshColliderComponent>(mesh);
    AddComponent<NavigationSourceComponent>(mesh_coll);
    transform_.position = Vec3(0, -0.5f, 0);
}

Slope::Slope(const Vec3& position)
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/test_stage.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDesc());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    MeshColliderComponent* mesh_coll = AddComponent<MeshColliderComponent>(mesh);
    AddComponent<NavigationSourceComponent>(mesh_coll);
    transform_.position = position - Vec3(0, 0.5f, 0);
}

#include "plane.h"
#include "../../../Engine/Components/static_mesh_component.h"
#include "../../../Engine/Components/mesh_collider_component.h"
#include "../../../Engine/Components/navigation_source_component.h"
Plane::Plane()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/plane.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDesc());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
     MeshColliderComponent* mesh_coll = AddComponent<MeshColliderComponent>(mesh);
    AddComponent<NavigationSourceComponent>(mesh_coll);
}

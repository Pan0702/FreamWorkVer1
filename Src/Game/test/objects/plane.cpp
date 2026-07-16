#include "plane.h"
#include "../../../Engine/Components/static_mesh_component.h"
Plane::Plane()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/plane.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
}

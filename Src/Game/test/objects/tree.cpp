#include "tree.h"
#include "../../../Engine/Components/static_mesh_component.h"
Tree::Tree(const Vec3& position)
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/tree.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = position;
}

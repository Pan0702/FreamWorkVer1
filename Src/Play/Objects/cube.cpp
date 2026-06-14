#include "cube.h"

#include "../../Engine/Components/static_mesh_component.h"
#include "../../Resource/mesh.h"
#include "../../Resource/mesh_manager.h"
#include "../../Resource/texture_manager.h"
#include "../../Resource/material_slot.h"

Cube::Cube()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/ground.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    AddComponent<MeshColliderComponent>(mesh);
    transform_.position = Vec3(0, 0, 0);
}

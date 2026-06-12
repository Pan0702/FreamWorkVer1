#include "cube.h"

#include "../../Engine/Components/static_mesh_component.h"
#include "../../Resource/mesh.h"
#include "../../Resource/mesh_manager.h"
#include "../../Resource/texture_manager.h"
#include "../../Resource/material_slot.h"
Cube::Cube()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/box1.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    materials_->GetMaterial(0)->SetDiffuse(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));
    materials_->GetMaterial(0)->SetNormal(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    auto* coll = AddComponent<MeshColliderComponent>(mesh);
    transform_.position = Vec3(0, 0, 0);
}

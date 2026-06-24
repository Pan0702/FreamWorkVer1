#include "goal_flag.h"
#include "../../Engine/Components/static_mesh_component.h"

GoalFlag::GoalFlag()
{
    Mesh* mesh2 = MeshManager::Get().Load("Assets/Mesh/GoalFlag.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh2->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh2, materials_.get());
    auto* coll = AddComponent<MeshColliderComponent>(mesh2);
    coll->SetOnBeginOverlap(this,&GoalFlag::OnBeginOverlap);
    transform_.scale = Vec3(0.02f, 0.02f, 0.02f);
}

void GoalFlag::OnBeginOverlap(const ColliderComponent* self, const ColliderComponent* other)
{
    DEBUG_LOG("GoalFlag");
}

#include "goal_flag.h"
#include "../../Engine/Components/static_mesh_component.h"
#include "../../Game/GameMain.h"
#include "../Player/player.h"
#include "../UI/clear.h"

GoalFlag::GoalFlag()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/GoalFlag.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    auto* m = AddComponent<MeshColliderComponent>(mesh);
    m->SetOnBeginOverlap(this,&GoalFlag::OnBeginOverlap);
    transform_.scale = Vec3(0.025f, 0.025f, 0.025f);
    transform_.position = Vec3(0, 0, 5);
}

GoalFlag::GoalFlag(const Vec3& pos)
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/GoalFlag.mesh");
    materials_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    AddComponent<StaticMeshComponent>(mesh, materials_.get());
    auto* m = AddComponent<MeshColliderComponent>(mesh);
    m->SetOnBeginOverlap(this,&GoalFlag::OnBeginOverlap);
    transform_.scale = Vec3(0.025f, 0.025f, 0.025f);
    transform_.position = pos;
}

void GoalFlag::OnBeginOverlap(const ColliderComponent* c1, const ColliderComponent* c2)
{
    Player* pl = dynamic_cast<Player*>(c2->GetOwner());
    if (!pl)
    {
        return;
    } 
    auto* clear = GetWorld()->FindActor<Clear>();
    pl->SetUseTick(false);
    clear->SetVisible(true);
    
}

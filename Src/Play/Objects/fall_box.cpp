#include "fall_box.h"

#include "../../Engine/Components/box_collider_component.h"
#include "../../Game/GameMain.h"
#include "../Player/player.h"
FallBox::FallBox()
{
    auto* box = AddComponent<BoxColliderComponent>();
    box->SetOnBeginOverlap(this,&FallBox::OnBeginOverlap);
    box->SetUseTransform(true);  
    box->SetHalfSize(Vec3(100.0f, 5.0f, 100.0f)); 
    box->SetTrigger(true);
    transform_.position = Vec3(0, -20.0f, 0);
}

void FallBox::OnBeginOverlap(const ColliderComponent* my, const ColliderComponent* other)
{
    Player* pl = dynamic_cast<Player*>(other->GetOwner());
    if (!pl)
    {
        return;
    }
    const std::string s = game_main->GetGameInstance().GetLevelManager().GetCurrentLevelName();
    game_main->GetGameInstance().GetLevelManager().OpenLevel(s);
}

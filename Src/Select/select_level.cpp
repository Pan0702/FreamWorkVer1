#include "select_level.h"
#include "UI/select.h"
void SelectLevel::OnEnter()
{
    SpawnActor<Select>();
    LevelBase::OnEnter();
}

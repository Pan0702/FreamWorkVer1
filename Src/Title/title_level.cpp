#include "title_level.h"

#include "UI/title.h"

void TitleLevel::OnEnter()
{
    SpawnActor<Title>();
    LevelBase::OnEnter();
}

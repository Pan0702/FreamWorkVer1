#include "level_factory.h"
#include "../Play/Stage/stage_1.h"
#include "../Play/Stage/stage_2.h"
#include "../Play/Stage/stage_3.h"
#include "../Play/Stage/stage_4.h"
#include "../Play/Stage/stage_5.h"
#include "../Play/Stage/stage_6.h"
#include "../Game/test_level.h"
#include "../Select/select_level.h"
#include "../Title/title_level.h"


void LevelFactory::Initialize()
{
    //ƒŒƒxƒ‹‚Ì“o˜^
    //exsamole
    //level_factory_.Register("Title", []
    //{
    //    return std::make_unique<TitleLevel>();
    //});
    Register<TestLevel>("Test");
    Register<TitleLevel>("Title");
    Register<Stage1>("Stage1");
    Register<Stage2>("Stage2");
    Register<Stage3>("Stage3");
    Register<Stage4>("Stage4");
    Register<Stage5>("Stage5");
    Register<Stage6>("Stage6");
    Register<SelectLevel>("Select");
}

std::unique_ptr<LevelBase> LevelFactory::Create(const std::string& name)
{
    auto it = factories_.find(name);
    if (it != factories_.end())
    {
        return it->second();
    }
    return nullptr;
}

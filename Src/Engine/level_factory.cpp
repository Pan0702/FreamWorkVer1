#include "level_factory.h"
#include "../Play/Stage/play_level.h"
#include "../Game/test_level.h"
#include "../Select/select_level.h"

void LevelFactory::Initialize()
{
    //ƒŒƒxƒ‹‚Ì“o˜^
    //exsamole
    //level_factory_.Register("Title", []
    //{
    //    return std::make_unique<TitleLevel>();
    //});
    Register<TestLevel>("Test");
    Register<PlayLevel>("Play");
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

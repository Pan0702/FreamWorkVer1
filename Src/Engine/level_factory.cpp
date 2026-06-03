#include "level_factory.h"

void LevelFactory::Initialize()
{
    //レベルの登録
    //exsamole
    //level_factory_.Register("Title", []
    //{
    //    return std::make_unique<TitleLevel>();
    //});
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

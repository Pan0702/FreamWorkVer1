#include "level_data.h"

#include <windows.h>

void LevelData::Initialize()
{
}

void LevelData::AddLevel(const std::string& name, std::unique_ptr<LevelBase> base)
{
    if (levels_.contains(name))
    {
        return;
    }
    levels_.emplace(name, std::move(base));
}

LevelBase* LevelData::Create(const std::string& name)
{
    auto it = levels_.find(name);
    if (it == levels_.end())
    {
        MessageBox(nullptr, L"Level is Not Found", L"Error", MB_OK);
        return nullptr;
    }
    return it->second.get();
}

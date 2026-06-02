#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "level_base.h"
class LevelData
{
public:
    void Initialize();
    void AddLevel(const std::string& name,std::unique_ptr<LevelBase> base);
    LevelBase* Create(const std::string& name);
private:
    std::unordered_map<std::string,std::unique_ptr<LevelBase>> levels_;
};

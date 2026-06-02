#pragma once
#include <memory>
#include <unordered_map>
#include <string>
class LevelBase;

class LevelData
{
public:
    void AddLevel();
private:
    std::unordered_map<std::string,std::unique_ptr<LevelBase>> levels_;
};

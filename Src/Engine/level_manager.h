#pragma once
#include <memory>
#include <string>

#include "level_base.h"

class LevelData;

class LevelManager
{
public:
    LevelManager();
    ~LevelManager();
    void Initialize();
    void Tick(float dt);
    void OpenLevel(const std::string& name);
private:
    std::string current_name_;
    std::string next_name_;
    LevelBase* current_level_ = nullptr;
    std::unique_ptr<LevelData> level_data_;
};
extern LevelManager* level_manager;
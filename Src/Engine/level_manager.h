#pragma once
#include <memory>
#include <string>

#include "level_base.h"
#include "level_factory.h"
class LevelManager
{
public:
    LevelManager();
    ~LevelManager();
    void Initialize(World* world);
    void Tick(float dt);
    void OpenLevel(const std::string& name);
private:
    void ApplyPendingLevel();
    std::string current_name_;
    std::string next_name_;
    World* world_ = nullptr;
    std::unique_ptr<LevelBase> current_level_ = nullptr;
    LevelFactory level_factory_;
};
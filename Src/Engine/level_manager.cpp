#include "level_manager.h"

#include "level_data.h"


LevelManager::LevelManager() = default;
LevelManager::~LevelManager() = default;

void LevelManager::Initialize()
{
    level_data_ = std::make_unique<LevelData>();
    level_data_->Initialize();
}

void LevelManager::Tick(float dt)
{
    if (current_name_ != next_name_)
    {
        current_name_ = next_name_;
        current_level_ = level_data_->Create(current_name_);
    }

    if (current_level_)
        current_level_->Tick(dt);
}

void LevelManager::OpenLevel(const std::string& name)
{
    next_name_ = name;
}

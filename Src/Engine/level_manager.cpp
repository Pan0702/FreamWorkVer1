#include "level_manager.h"

#include "level_factory.h"


LevelManager::LevelManager() = default;
LevelManager::~LevelManager() = default;

void LevelManager::Initialize(World* world)
{
    world_ = world;
    level_factory_.Initialize();
}

void LevelManager::Tick(float dt)
{
    if (current_name_ != next_name_)
    {
        ApplyPendingLevel();
    }

    if (current_level_)
        current_level_->Tick(dt);
}

void LevelManager::OpenLevel(const std::string& name)
{
    next_name_ = name;
}

void LevelManager::ApplyPendingLevel()
{
    if (next_name_.empty())
    {
        return;
    }
    
    if (next_name_ == current_name_)
    {
        next_name_.clear();
        return;
    }
    
    if (current_level_)
    {
        current_level_->OnExit();
    }
    current_name_ = next_name_;
    next_name_.clear();   
    current_level_ = level_factory_.Create(current_name_);
    
    if (current_level_)
    {
        current_level_->SetWorld(world_);  
        current_level_->OnEnter();
    }
}

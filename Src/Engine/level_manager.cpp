#include "level_manager.h"

#include "level_factory.h"


LevelManager::LevelManager() = default;
LevelManager::~LevelManager() = default;

void LevelManager::Initialize(World* world)
{
    world_ = world;
    level_factory_.Initialize();
    transition_.SetEffect(std::make_unique<Fade>());
}

void LevelManager::Tick(float dt)
{
    transition_.Tick(dt);
    if (transition_.GetJustCovered())
    {
        ApplyPendingLevel();
    }

    if (current_level_)
    {
        current_level_->Tick(dt);
    }
    
    transition_.Draw();
}

void LevelManager::OpenLevel(std::string_view name)
{
    if (transition_.IsPlaying())
    {
        return;
    }
    next_name_ = name;
    transition_.Start();
}

const std::string& LevelManager::GetCurrentLevelName() const
{
    return current_name_;
}

void LevelManager::ApplyPendingLevel()
{
    
    if (next_name_.empty())
    {
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

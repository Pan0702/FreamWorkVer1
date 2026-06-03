#pragma once
#include <functional>
#include <memory>
#include <string>
#include "level_base.h"

class LevelFactory
{
public:
    void Initialize();
    using CreateFunc = std::function<std::unique_ptr<LevelBase>()>;
    
    template<class T>
    void Register(const std::string& name)
    {
        static_assert(std::is_base_of_v<LevelBase, T>);
        factories_[name] = []{return std::make_unique<T>();};
    }
    std::unique_ptr<LevelBase> Create(const std::string& name);
    
private:
    std::unordered_map<std::string, CreateFunc> factories_;
};

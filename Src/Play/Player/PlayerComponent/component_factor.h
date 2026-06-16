#pragma once
#include <memory>
#include <unordered_map>
#include <string>
enum class PlayerState : uint8_t;
class StateComponentBase;

class ComponentFactor
{
public:
    ComponentFactor();
    using ComponentMap = std::unordered_map<PlayerState,StateComponentBase*>;
    ComponentMap GetComponents(const std::vector<PlayerState>& names) const; 
private:
    std::unordered_map<PlayerState,std::unique_ptr<StateComponentBase>> components_;
};

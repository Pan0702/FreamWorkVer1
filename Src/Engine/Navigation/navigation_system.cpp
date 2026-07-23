#include "navigation_system.h"
#include <algorithm>
uint32 NavigationSystem::RegisterSource(NavigationSourceComponent* component)
{
    if (component == nullptr)
    {
        return 0;
    }
    for (const auto& s : sources_)
    {
        if (s.component == component)
        {
            return s.id;
        }
    }
    
    RegisteredSource r = {};
    r.id = next_id_;
    r.component = component;
    sources_.push_back(r);
    next_id_++;
    return r.id;
}

void NavigationSystem::UnregisterSource(uint32 source_id)
{
    if (source_id == 0)
    {
        return;
    }
    auto it = std::ranges::find(sources_, source_id, &RegisteredSource::id);
    if (it != sources_.end())
    {
        sources_.erase(it);
    }
}

#include "component_factor.h"

#include "Jump.h"
#include "state_component.h"
#include "../player.h"
#include "Walk.h"

ComponentFactor::ComponentFactor()
{
    components_.emplace(PlayerState::kJump,std::make_unique<PlayerJump>());
    components_.emplace(PlayerState::kWalk,std::make_unique<PlayerWalk>());
    components_.emplace(PlayerState::kIdle,std::make_unique<PlayerStand>());
}


ComponentFactor::ComponentMap ComponentFactor::GetComponents(const std::vector<PlayerState>& names) const
{
    ComponentMap tmp;
    for (PlayerState state : names)
    {
        if (components_.contains(state))
        {
            tmp.emplace(state, components_.at(state).get());
        }
    }
    return tmp;
}

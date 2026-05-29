#include "actor.h"

Actor::~Actor()
{
    Detach();
}

void Actor::Attach(const AttachContext& context)
{
    if (attached_)
    {
        return;
    }

    for (const auto& component : components_)
    {
        component->OnAttach(context);
    }
    attached_ = true;
}

void Actor::Detach()
{
    if (!attached_)
    {
        return;
    }

    for (auto it = components_.rbegin(); it != components_.rend(); ++it)
    {
        (*it)->OnDetach();
    }
    attached_ = false;
}

void Actor::Tick(float dt)
{
    for (const auto& component : components_)
    {
        component->Tick(dt);
    }
}

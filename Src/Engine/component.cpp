#include "component.h"

void Component::OnAttach(const AttachContext& context)
{
    (void)context;
}

void Component::OnDetach()
{
}

void Component::Tick(float dt)
{
    (void)dt;
}

Actor* Component::GetOwner() const
{
    return owner_;
}

void Component::SetOwner(Actor* owner)
{
    owner_ = owner;
}

#include "world.h"

#include <algorithm>

#include "actor.h"

Actor* World::AddActor(std::unique_ptr<Actor> actor)
{
    Actor* result = actor.get();
    result->Attach(attach_context_);
    actors_.push_back(std::move(actor));
    return result;
}

void World::RemoveActor(Actor* actor)
{
    auto it = std::ranges::find_if(actors_,
                                   [actor](const std::unique_ptr<Actor>& candidate)
                                   {
                                       return candidate.get() == actor;
                                   });
    if (it != actors_.end())
    {
        (*it)->Detach();
        actors_.erase(it);
    }
}

void World::SetAttachContext(const AttachContext& context)
{
    attach_context_ = context;
}

void World::Tick(float dt)
{
    for (const auto& actor : actors_)
    {
        actor->Tick(dt);
    }
}

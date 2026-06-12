#include "world.h"

#include <algorithm>

#include "actor.h"


void World::RegisterActor(Actor* actor)
{
    actors_.push_back(actor);
}

void World::UnregisterActor(Actor* actor)
{
    std::erase(actors_, actor);
}

void World::SetAttachContext(const AttachContext& context)
{
    attach_context_ = context;
    attach_context_.collision_world = &collision_world_;
}

AttachContext World::GetAttachContext() const
{
    return attach_context_;
}

void World::Tick(float dt)
{
    for (const auto& actor : actors_)
    {
        actor->Tick(dt);
    }
    collision_world_.Collect();
    collision_world_.DrawDebug();
}

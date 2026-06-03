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
}

void World::Tick(float dt)
{
    for (const auto& actor : actors_)
    {
        actor->Tick(dt);
    }
}

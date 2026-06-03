#pragma once
#include <vector>

#include "attach_context.h"

class Actor;

class World
{
public:
    void RegisterActor(Actor* actor);
    void UnregisterActor(Actor* actor);
    void SetAttachContext(const AttachContext& context);
    AttachContext GetAttachContext() const;
    void Tick(float dt);

private:
    std::vector<Actor*> actors_;
    AttachContext attach_context_ = {};
};

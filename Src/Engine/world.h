#pragma once
#include <memory>
#include <vector>

#include "attach_context.h"

class Actor;

class World
{
public:
    Actor* AddActor(std::unique_ptr<Actor> actor);
    void RemoveActor(Actor* actor);
    void SetAttachContext(const AttachContext& context);
    void Tick(float dt);

private:
    std::vector<std::unique_ptr<Actor>> actors_;
    AttachContext attach_context_ = {};
};

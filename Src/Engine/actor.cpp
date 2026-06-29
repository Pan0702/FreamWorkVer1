#include "actor.h"
#include "world.h"

Actor::~Actor()
{
    Detach();
    if (world_)
    {
        world_->UnregisterActor(this);
        world_ = nullptr;
    }
}

void Actor::OnSpawn(World* world)
{
    world_ = world;
    if (world_)
    {
        world_->RegisterActor(this);
        // ワールドに入った時点で actor をアタッチ状態にする。
        // これで以降の AddComponent が即座に OnAttach を呼び、
        // StaticMeshComponent 等が各レンダラーに登録される。
        Attach(world_->GetAttachContext());
    }
    Begin();
}

World* Actor::GetWorld() const
{
    return world_;
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
    if (!use_tick_)
    {
        return;
    }
    for (const auto& component : components_)
    {
            component->Tick(dt);
    }
}

void Actor::SetUseTick(bool use)
{
    use_tick_ = use;
}

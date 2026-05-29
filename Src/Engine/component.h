#pragma once

struct AttachContext;
class Actor;

class Component
{
public:
    Component() = default;
    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    virtual void OnAttach(const AttachContext& context);
    virtual void OnDetach();
    virtual void Tick(float dt);

    Actor* GetOwner() const;

private:
    friend class Actor;

    void SetOwner(Actor* owner);

    Actor* owner_ = nullptr;
};

#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>
#include "world.h"
#include "attach_context.h"
#include "component.h"


struct AttachContext;

class Actor
{
public:
    Actor() = default;
    virtual ~Actor();

    virtual void Begin()
    {
    }

    void OnSpawn(World* world);
    World* GetWorld() const;

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

    template <class T, class... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>);

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* result = component.get();
        result->SetOwner(this);
        components_.push_back(std::move(component));
        if (attached_ && world_ != nullptr)
        {
            result->OnAttach(world_->GetAttachContext());
        }
        return result;
    }

    template <class T>
    T* GetComponent() const
    {
        static_assert(std::is_base_of_v<Component, T>);

        for (const auto& component : components_)
        {
            if (auto* result = dynamic_cast<T*>(component.get()))
            {
                return result;
            }
        }
        return nullptr;
    }

    void Tick(float dt);

private:
    void Attach(const AttachContext& context);
    void Detach();

    World* world_ = nullptr;
    std::vector<std::unique_ptr<Component>> components_;
    bool attached_ = false;
};

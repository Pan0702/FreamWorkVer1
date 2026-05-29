#pragma once
#include <memory>
#include <type_traits>
#include <vector>

#include "attach_context.h"
#include "component.h"

class Actor
{
public:
    Actor() = default;
    ~Actor();

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

    void Attach(const AttachContext& context);
    void Detach();
    void Tick(float dt);

private:
    std::vector<std::unique_ptr<Component>> components_;
    bool attached_ = false;
};

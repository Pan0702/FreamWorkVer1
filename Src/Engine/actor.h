#pragma once
#include <memory>
#include <type_traits>
#include <vector>
#include <utility>
#include "world.h"
#include "attach_context.h"
#include "component.h"
#include "Components/transform_component.h"
#include "../Resource/material_slot.h"
#include "../Resource/texture_manager.h"
#include "../Resource/mesh_manager.h"
#include "../Resource/animator_manager.h"
#include "../Resource/skeltal_mesh_manager.h"

struct AttachContext;

/**
 * @brief Actorのデータと処理をまとめる型。
 */
class Actor
{
public:
    Actor() = default;
    virtual ~Actor();

    // 生成直後の初期処理。派生クラスで必要な初期化を行う。
    virtual void Begin()
    {
    }

    /**
     * @brief OnSpawnを行う関数。
     * @param world 引数。
     */
    void OnSpawn(World* world);
    /**
     * @brief Worldを取得する関数。
     * @return 戻り値。
     */
    World* GetWorld() const;

    // 全 Actor が必ず持つ Transform。Component 配列ではなく直接保持する。
    TransformComponent& GetTransform() { return transform_; }
    const TransformComponent& GetTransform() const { return transform_; }

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

    // T: 追加する Component 派生型。args: コンストラクタ引数。戻り値: 追加したコンポーネント。
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

    // T: 探す Component 派生型。戻り値: 最初に見つかったコンポーネント。なければ nullptr。
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

    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    virtual void Tick(float dt);

protected:
    TransformComponent transform_;

private:
    /**
     * @brief Attachを行う関数。
     * @param context 共有コンテキスト。
     */
    void Attach(const AttachContext& context);
    /**
     * @brief Detachを行う関数。
     */
    void Detach();

    World* world_ = nullptr;
    std::vector<std::unique_ptr<Component>> components_;
    bool attached_ = false;
};

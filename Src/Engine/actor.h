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
    /**
     * @brief 値を初期化する。
     */
    Actor() = default;
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    virtual ~Actor();

    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    virtual void Begin()
    {
    }

    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     * @param world Actor や描画対象を管理する World。
     */
    void OnSpawn(World* world);
    /**
     * @brief Actor が所属している World を取得する。
     * @return 所属中の World。見つからない、または未作成の場合は nullptr。
     */
    World* GetWorld() const;

    /**
     * @brief Actor が持つ Transform を取得する。
     * @return 保持している Transform への参照。
     */
    TransformComponent& GetTransform() { return transform_; }
    /**
     * @brief Actor が持つ Transform を取得する。
     * @return 保持している Transform への参照。
     */
    const TransformComponent& GetTransform() const { return transform_; }

    /**
     * @brief 値を初期化する。
     */
    Actor(const Actor&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    Actor& operator=(const Actor&) = delete;

    /**
     * @brief Component 派生型を生成し、Actor の所有物として登録する。
     * @param args 生成する型へ渡すコンストラクタ引数。
     * @return 生成して Actor に登録した Component。
     */
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

    /**
     * @brief Actor が所有する Component から指定型の最初の要素を探す。
     * @return 指定型の Component。見つからない場合は nullptr。
     */
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
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    virtual void Tick(float dt);

protected:
    TransformComponent transform_;

private:
    /**
     * @brief 所有中の Component を共有コンテキストへ接続する。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void Attach(const AttachContext& context);
    /**
     * @brief 所有中の Component を逆順で共有コンテキストから外す。
     */
    void Detach();

    World* world_ = nullptr;
    std::vector<std::unique_ptr<Component>> components_;
    bool attached_ = false;
};

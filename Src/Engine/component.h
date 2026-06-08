#pragma once

struct AttachContext;
class Actor;

/**
 * @brief Componentのデータと処理をまとめる型。
 */
class Component
{
public:
    Component() = default;
    virtual ~Component() = default;

    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;

    /**
     * @brief OnAttachを行う関数。
     * @param context 共有コンテキスト。
     */
    virtual void OnAttach(const AttachContext& context);
    /**
     * @brief OnDetachを行う関数。
     */
    virtual void OnDetach();
    /**
     * @brief Tickを行う関数。
     * @param dt 引数。
     */
    virtual void Tick(float dt);

    /**
     * @brief Ownerを取得する関数。
     * @return 戻り値。
     */
    Actor* GetOwner() const;

private:
    friend class Actor;

    /**
     * @brief Ownerを設定する関数。
     * @param owner 引数。
     */
    void SetOwner(Actor* owner);

    Actor* owner_ = nullptr;
};

#pragma once

struct AttachContext;
class Actor;

/**
 * @brief Componentのデータと処理をまとめる型。
 */
class Component
{
public:
    /**
     * @brief 値を初期化する。
     */
    Component() = default;
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    virtual ~Component() = default;

    /**
     * @brief 値を初期化する。
     */
    Component(const Component&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    Component& operator=(const Component&) = delete;

    /**
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    virtual void OnAttach(const AttachContext& context);
    /**
     * @brief 共有コンテキストから登録を外し、システム側の参照を切る。
     */
    virtual void OnDetach();
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    virtual void Tick(float dt);

    /**
     * @brief 所有している Actor を取得する。
     * @return 所有している Actor。見つからない、または未作成の場合は nullptr。
     */
    Actor* GetOwner() const;

private:
    friend class Actor;

    /**
     * @brief 指定された値を内部状態に反映する。
     * @param owner このコンポーネントを所有するアクター。
     */
    void SetOwner(Actor* owner);

    Actor* owner_ = nullptr;
};

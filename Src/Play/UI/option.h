#pragma once
#include "../../Engine/actor.h"
#include "../../Engine/game_instance.h"

/**
 * @brief オプションメニューで選択できる操作を表す。
 */
enum class OptionButton : uint8
{
    kRestart = 0,
    kSelect,
    kExit,
};
class SpriteComponent;
/**
 * @brief ゲーム中のオプションメニューを管理する。
 */
class Option : public GameInstance
{
public:
    /**
     * @brief オプションメニューのリソースを生成する。
     */
    Option();
    /**
     * @brief オプションメニューのリソースを解放する。
     */
    ~Option() override;
    /**
     * @brief このメニューを共有エンジンサービスへ登録する。
     * @param context メニューが使用するサービス。
     */
    void OnAttach(const AttachContext& context) const;
    /**
     * @brief このメニューを共有エンジンサービスから解除する。
     */
    void OnDetach() const;
    /**
     * @brief メニューの表示状態と入力を更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    /**
     * @brief オプションメニューが表示中かどうかを返す。
     * @return オプションメニューを表示中の場合は true。
     */
    bool IsVisible() const;
    /**
     * @brief オプションメニューを表示または非表示にする。
     * @param visible メニューを表示する場合は true。
     */
    void SetVisible(bool visible);

private:
    /**
     * @brief プレイヤー入力に応じて選択中の項目を変更する。
     */
    void Input();
    /**
     * @brief オプションメニューで選択された操作を実行する。
     */
    void SetOpen();
    std::unique_ptr<SpriteComponent> ui_ = nullptr;
    std::unique_ptr<SpriteComponent> cur_texture_ = nullptr;
    std::unique_ptr<SpriteComponent> overlay_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
    int button_index_ = 0;
    bool visible_ = false;
};

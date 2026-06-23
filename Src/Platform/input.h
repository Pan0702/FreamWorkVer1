#pragma once
#include "key_code.h"
#include "Window.h"

/**
 * @brief Inputのデータと処理をまとめる型。
 */
class Input
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param hwnd 描画対象の Win32 ウィンドウハンドル。
     */
    void Initialize(HWND hwnd);
    /**
     * @brief 1 フレーム分の状態更新を進める。
     */
    void Update();
    /**
     * @brief キーの現在状態と直前状態から入力状態を判定する。
     * @param keycode keycode に設定する値。
     * @param key key に設定する値。
     * @return 指定したキーが押されている場合は true。
     */
    bool CheckKey(uint8_t keycode, KeyState key) const;
    /**
     * @brief マウスボタンの現在状態と直前状態から入力状態を判定する。
     * @param button button に設定する値。
     * @param key key に設定する値。
     * @return 指定したマウスボタンが押されている場合は true。
     */
    bool CheckMouseButton(uint8_t button, KeyState key) const;
    /**
     * @brief マウス X 座標 を取得する。
     * @return 現在保持している マウス X 座標。
     */
    int GetMouseX() const;
    /**
     * @brief マウス Y 座標 を取得する。
     * @return 現在保持している マウス Y 座標。
     */
    int GetMouseY() const;
    /**
     * @brief マウスの X 方向移動量を取得する。
     * @return 前回更新からのマウス X 方向移動量。
     */
    int GetMouseDeltaX() const;
    /**
     * @brief マウスの Y 方向移動量を取得する。
     * @return 前回更新からのマウス Y 方向移動量。
     */
    int GetMouseDeltaY() const;

private:
    bool current_keys_[256] = {};
    bool previous_keys_[256] = {};
    int mouse_x_ = 0, mouse_y_ = 0;
    int mouse_delta_x_ = 0, mouse_delta_y_ = 0;
    bool mouse_buttons_[5] = {};
    bool mouse_buttons_previous_[5] = {};
    UINT key_to_vk_[256] = {};
    HWND hwnd_ = nullptr;
};

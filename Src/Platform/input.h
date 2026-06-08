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
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param hwnd Win32 ウィンドウハンドル。
     */
    void Initialize(HWND hwnd);
    /**
     * @brief 1 フレーム分の状態を更新する関数。
     */
    void Update();
    /**
     * @brief CheckKeyを行う関数。
     * @param keycode 引数。
     * @param key 引数。
     * @return 条件を満たす場合は true。
     */
    bool CheckKey(uint8_t keycode, KeyState key) const;
    /**
     * @brief CheckMouseButtonを行う関数。
     * @param button 引数。
     * @param key 引数。
     * @return 条件を満たす場合は true。
     */
    bool CheckMouseButton(uint8_t button, KeyState key) const;
    /**
     * @brief MouseXを取得する関数。
     * @return 戻り値。
     */
    int GetMouseX() const;
    /**
     * @brief MouseYを取得する関数。
     * @return 戻り値。
     */
    int GetMouseY() const;
    /**
     * @brief MouseDeltaXを取得する関数。
     * @return 戻り値。
     */
    int GetMouseDeltaX() const;
    /**
     * @brief MouseDeltaYを取得する関数。
     * @return 戻り値。
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

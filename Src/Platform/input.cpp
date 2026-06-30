#include "input.h"

void Input::Initialize(HWND hwnd)
{
    hwnd_ = hwnd;
    constexpr int key_max_num = 256;
    for (int i = 0; i < key_max_num; ++i)
    {
        key_to_vk_[i] = MapVirtualKey(i, MAPVK_VSC_TO_VK);
    }
    ShowCursor(false);
}

void Input::Update()
{
    memcpy(previous_keys_, current_keys_, sizeof(current_keys_));
    memcpy(mouse_buttons_previous_, mouse_buttons_, sizeof(mouse_buttons_));

    BYTE keyboard_state[256] = {};
    if (GetKeyboardState(keyboard_state))
    {
        constexpr int key_max_num = 256;
        for (int i = 0; i < key_max_num; ++i)
        {
            const UINT vk = key_to_vk_[i];
            current_keys_[i] = (vk != 0) && ((keyboard_state[vk] & 0x80) != 0);
        }
    }
    else
    {
        memset(current_keys_, 0, sizeof(current_keys_));
    }
    if (mouse_locked_)
    {
        RECT rc;
        GetClientRect(hwnd_, &rc);
        POINT center = {rc.right / 2, rc.bottom / 2};

        POINT cur;
        GetCursorPos(&cur);
        ScreenToClient(hwnd_, &cur);
        // 中央からのズレ＝今フレームの移動量//
        mouse_delta_x_ = cur.x - center.x;
        mouse_delta_y_ = cur.y - center.y;

        POINT c = center;
        ClientToScreen(hwnd_, &c);
        // 毎フレーム中央へ戻す//
        SetCursorPos(c.x, c.y);
        mouse_x_ = center.x;
        mouse_y_ = center.y;
    }
    else
    {
        int prev_x = mouse_x_;
        int prev_y = mouse_y_;

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hwnd_, &pt);
        mouse_x_ = pt.x;
        mouse_y_ = pt.y;
        mouse_delta_x_ = mouse_x_ - prev_x;
        mouse_delta_y_ = mouse_y_ - prev_y;
    }
    mouse_buttons_[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    mouse_buttons_[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    mouse_buttons_[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
    mouse_buttons_[3] = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0;
    mouse_buttons_[4] = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
}

bool Input::CheckKey(uint8_t keycode, KeyState key) const
{
    const bool now = current_keys_[keycode];
    const bool prev = previous_keys_[keycode];
    switch (key)
    {
    case KeyState::kDown: return now;
    case KeyState::kReleased: return !now && prev;
    case KeyState::kPressed: return now && !prev;
    }
    return false;
}

bool Input::CheckMouseButton(uint8_t button, KeyState key) const
{
    if (button >= 5)
    {
        return false;
    }
    const bool now = mouse_buttons_[button];
    const bool prev = mouse_buttons_previous_[button];
    switch (key)
    {
    case KeyState::kDown: return now;
    case KeyState::kReleased: return !now && prev;
    case KeyState::kPressed: return now && !prev;
    }
    return false;
}

int Input::GetMouseX() const
{
    return mouse_x_;
}

int Input::GetMouseY() const
{
    return mouse_y_;
}

int Input::GetMouseDeltaX() const
{
    return mouse_delta_x_;
}

int Input::GetMouseDeltaY() const
{
    return mouse_delta_y_;
}

void Input::SetMouseLock(bool lock)
{
    if (lock == mouse_locked_) return;
    mouse_locked_ = lock;
    
}

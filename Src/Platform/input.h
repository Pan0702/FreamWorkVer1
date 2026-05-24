#pragma once
#include "key_code.h"
#include "Window.h"

class Input
{
public:
    void Initialize(HWND hwnd);
    void Update();
    bool CheckKey(uint8_t keycode,KeyState key) const;
    bool CheckMouseButton(uint8_t button, KeyState key) const;
    int GetMouseX() const;
    int GetMouseY() const;
    int GetMouseDeltaX() const;
    int GetMouseDeltaY() const;
    
private:
    bool current_keys_[256] = {};
    bool previous_keys_[256] = {};
    int mouse_x_ = 0,mouse_y_ = 0;
    int mouse_delta_x_ = 0,mouse_delta_y_ = 0;
    bool mouse_buttons_[5] = {};
    bool mouse_buttons_previous_[5] = {};
    HWND hwnd_ = nullptr;
};

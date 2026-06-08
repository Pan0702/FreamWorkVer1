#pragma once
#pragma execution_character_set("shift_jis")
#include <functional>

#include "../Core/common.h"

constexpr uint32_t kFrameCount = 2;

struct WindowSize
{
    uint32_t width;
    uint32_t height;
};

class Window
{
public:
    Window();
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool Create(const wchar_t* title, uint32_t width, uint32_t height);
    void Show() const;
    bool ProcessMessages() const;

    HWND GetHwnd() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    WindowSize GetSize() const;
    uint32_t GetLastMessageCount() const;
    uint32_t GetLastKeyMessageCount() const;
    void DispFPS(const wchar_t* title);

    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    void SetResizeCallback(ResizeCallback callback);

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    WindowSize size_;
    bool closed_ = false;
    mutable uint32_t last_message_count_ = 0;
    mutable uint32_t last_key_message_count_ = 0;
    std::wstring class_name_ = L"FrameWorkWindowClass";
    ResizeCallback resize_callback_ = nullptr;
};
#include "Window.h"

Window::Window()
{
}

Window::~Window()
{
    if (hwnd_ != nullptr)
    {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }

    UnregisterClass(class_name_.c_str(), instance_);
}

bool Window::Create(const wchar_t* title, uint32_t width, uint32_t height)
{
    instance_ = GetModuleHandle(nullptr); 
    size_.height = height;
    size_.width = width;
    // CreateWindowExW??g???E?B???h?E?N???X????o?^????B
    WNDCLASSEXW window_class = {};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = WndProc;
    window_class.hInstance = instance_;
    window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
    window_class.lpszClassName = class_name_.c_str();
    if (!RegisterClassEx(&window_class))
    {
        return false;
    }

    // ??„‘o???????w?????`????T?C?Y???AWin32??RECT????????B
    RECT rect = {
        0, 0,
        static_cast<LONG>(width), static_cast<LONG>(height)
    };

    // CreateWindowEx??n??????A?O?g??^?C?g???o?[?????T?C?Y???????B
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;
    
    // WndProc?????C++?I?u?W?F?N?g?????o??????A?????????this??n???B
    hwnd_ = CreateWindowEx(0, class_name_.c_str(), title,
                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           window_width, window_height, nullptr, nullptr, instance_, this);

    return hwnd_ != nullptr;
}

void Window::Show() const
{
    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
}

bool Window::ProcessMessages() const
{
    MSG msg = {};
    // ???b?Z?[?W???????????C?????[?v???~????????APeekMessage???g???B
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        // WM_QUIT??????????A?A?v???P?[?V????????C?????[?v???I??????B
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return !closed_;
}

HWND Window::GetHwnd() const
{
    return hwnd_;
}

uint32_t Window::GetWidth() const
{
    return size_.width;
}

uint32_t Window::GetHeight() const
{
    return size_.height;
}

WindowSize Window::GetSize() const
{
    return size_;
}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;
    // ???????????b?Z?[?W???ACreateWindowEx??n????this???????????B
    if (msg == WM_NCCREATE)
    {
        auto* create_struct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        window = static_cast<Window*>(create_struct->lpCreateParams);
        // ?????~????b?Z?[?W??g??????AHWND??this????????B
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd_ = hwnd;
    }
    else
    {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window != nullptr)
    {
        return window->HandleMessage(msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // ???????????????Win32???b?Z?[?W??????????????B
    switch (msg)
    {
    case WM_CLOSE:
        closed_ = true;
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
        return 0;
    case WM_DESTROY:
        closed_ = true;
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        size_.width = LOWORD(lParam);
        size_.height = HIWORD(lParam);
        return 0;
    }
    // ??????????b?Z?[?W??Win32??W????????C????B
    return DefWindowProc(hwnd_, msg, wParam, lParam);
}

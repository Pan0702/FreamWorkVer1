
#include <tchar.h>

#include "Window.h"
#include "../Debug/ImGui/imgui_impl_win32.h"
#include "../Core/common.h"
#include "../Game/GameMain.h"

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
    
    // CreateWindowExWを呼ぶ前にウィンドウクラスを登録する
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

    // 呼び出し側から指定された描画サイズから、Win32用のRECTを設定する
    RECT rect = {
        0, 0,
        static_cast<LONG>(width), static_cast<LONG>(height)
    };

    // CreateWindowExに渡すために、外枠やタイトルバーを含めたウィンドウサイズに補正する
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;
    
    // WndProcからC++オブジェクトを取り出せるように、最後の引数にthisを渡す
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
    last_message_count_ = 0;
    last_key_message_count_ = 0;
    // メッセージがなくてもメインループを止めないようにPeekMessageを使う
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ++last_message_count_;
        if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP ||
            msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP ||
            msg.message == WM_CHAR)
        {
            ++last_key_message_count_;
        }

        // WM_QUITを受信したら、アプリケーションのメインループを終了する
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

uint32_t Window::GetLastMessageCount() const
{
    return last_message_count_;
}

uint32_t Window::GetLastKeyMessageCount() const
{
    return last_key_message_count_;
}

void Window::DispFPS(const wchar_t* title) const
{
    SetWindowText(hwnd_, title);
}

void Window::SetResizeCallback(const ResizeCallback& callback)
{
    resize_callback_ = std::move(callback);
}

void Window::Close()
{
    PostQuitMessage(0);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM,LPARAM);
LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //ImGuiでマウス操作を可能にする。
    LRESULT lr = ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
    if (lr)
    {
        return true;
    }
    Window* window = nullptr;
    // 一番最初のメッセージ（WM_NCCREATE）の時に、CreateWindowExに渡したthisを受け取る
    if (msg == WM_NCCREATE)
    {
        auto* create_struct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        window = static_cast<Window*>(create_struct->lpCreateParams);
        // 次回以降のメッセージで使い回せるように、HWNDにthisを紐付ける
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
    // ここで必要なWin32メッセージを個別に処理する
    switch (msg)
    {
    case WM_CLOSE:
        closed_ = true;
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
        return 0;
    case WM_SETFOCUS:
        //マウスのカーソルを固定//
        game_main->GetInput().SetMouseLock(true);
        break;
    case WM_KILLFOCUS:
        //マウスのカーソルの固定を解除//
        game_main->GetInput().SetMouseLock(false);
        break;
    case WM_DESTROY:
        closed_ = true;
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        {
            uint32_t w = LOWORD(lParam);
            uint32_t h = HIWORD(lParam);
            if (w == 0 || h == 0)
                return 0;
        
            if ( size_.width == w && size_.height == h)
                return 0;
            size_.width = w;
            size_.height = h;
            if (resize_callback_)
                resize_callback_(w,h);
            return 0;
        }
    }
    // 処理しなかったメッセージはWin32の標準処理に任せる
    return DefWindowProc(hwnd_, msg, wParam, lParam);
}

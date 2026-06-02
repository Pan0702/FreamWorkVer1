#pragma once
#pragma execution_character_set("shift_jis")
#include <functional>

#include "../Core/common.h"

constexpr uint32_t kFrameCount = 2;

// ウィンドウのサイズをまとめるstruct
struct WindowSize
{
    uint32_t width;
    uint32_t height;
};

// Win32ウィンドウ全体を管理するクラス
class Window
{
public:
    Window();
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    // ウィンドウを作成する
    // title: タイトルバーに表示する文字列
    // width: ウィンドウの横幅
    // height: ウィンドウの縦幅
    // 返り値: 作成できたらtrue
    bool Create(const wchar_t* title, uint32_t width, uint32_t height);

    // ウィンドウを表示する
    void Show() const;

    // 毎フレームWindowsイベントを処理する
    bool ProcessMessages() const;

    // ウィンドウハンドルを取得する
    HWND GetHwnd() const;

    // ウィンドウの横のサイズを取得する
    uint32_t GetWidth() const;

    // ウィンドウの縦のサイズを取得する
    uint32_t GetHeight() const;

    // ウィンドウのサイズを取得する
    WindowSize GetSize() const;

    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    void SetResizeCallback(ResizeCallback callback);
private:
    // Win32から呼び出されるメッセージ受信用の関数
    // HWNDに保存されたthisを取り出して、HandleMessage()関数へ渡す。
    // hwnd: 通知先ウィンドウ
    // msg: メッセージ種類
    // wParam: msgごとの追加情報1
    // lParam: msgごとの追加情報2
    // return: メッセージの処理結果
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // WndProcから渡されたメッセージを実際に処理する
    // msg: メッセージ種類
    // wParam: msgごとの追加情報1
    // lParam: msgごとの追加情報2
    // return: メッセージの処理結果
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    HINSTANCE instance_ = nullptr;
    HWND hwnd_ = nullptr;
    WindowSize size_ = {};
    bool closed_ = false;
    std::wstring class_name_ = L"FrameWorkWindowClass";
    ResizeCallback resize_callback_ = nullptr;
};
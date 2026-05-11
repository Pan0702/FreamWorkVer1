#pragma once
#include "../Core/common.h"

namespace fw
{
    constexpr uint32_t KFrameCount = 2;

    //描画領域サイズをまとめるstruct
    struct WindowSize
    {
        uint32_t width;
        uint32_t height;
    };

    //Win32ウィンドウ全体を管理するクラス
    class Window
    {
    public:
        Window();
        ~Window();
        // ウィンドウを作成する
        //　title:タイトルバーに表示する文字列
        //　width:ウィンドウの横幅
        //　height:ウィンドウの縦幅
        //  作成できたらtrue
        bool Create(const wchar_t* title, int width, int height);

        // ウィンドウを描画する
        void Show();

        //　舞フレームWindowsイベントを処理する
        bool ProcessMessages();

        // ウィンドウハンドルを取得する
        HWND GetHwnd() const;

        // ウィンドウの横のサイズを取得する
        uint32_t GetWidth() const;

        // ウィンドウの縦のサイズを取得する
        uint32_t GetHeight() const;

        // ウィンドウのサイズを取得する
        WindowSize GetSize() const;

    private:
        // Win32から呼ばれるメッセージ受信用の入口
        // HWNDに保存したthisを取り出して、HandleMessage()へ処理を渡す。
        // hwnd:通知先ウィンドウ
        // msg:メッセージ種類
        // wParam:msgごとの追加情報１
        // lParam:msgごとの追加情報２
        // return:メッセージの処理結果
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        // WndProcから渡されたメッセージを実際に処理する
        // msg:メッセージ種類
        // wParam:msgごとの追加情報１
        // lParam:msgごとの追加情報２
        // return:メッセージの処理結果
        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

        HINSTANCE instance_ = nullptr;
        HWND hwnd_ = nullptr;
        WindowSize size_ = {};
        bool closed_ = false;
        std::wstring class_name_ = L"FrameWorkWindowClass";
    };
}

using namespace fw;

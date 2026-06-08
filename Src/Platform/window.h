#pragma once
#include <functional>

#include "../Core/common.h"

// ダブルバッファリングで使うフレーム数。
constexpr uint32_t kFrameCount = 2;

/**
 * @brief WindowSizeのデータと処理をまとめる型。
 */
struct WindowSize
{
    uint32_t width;
    uint32_t height;
};

/**
 * @brief Windowのデータと処理をまとめる型。
 */
class Window
{
public:
    /**
     * @brief Windowを初期化するコンストラクタ。
     */
    Window();
    /**
     * @brief Windowの終了処理を行うデストラクタ。
     */
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    /**
     * @brief DirectX 12 リソースを作成する関数。
     * @param title 引数。
     * @param width 幅。
     * @param height 高さ。
     * @return 条件を満たす場合は true。
     */
    bool Create(const wchar_t* title, uint32_t width, uint32_t height);
    /**
     * @brief Showを行う関数。
     */
    void Show() const;
    /**
     * @brief ProcessMessagesを行う関数。
     * @return 条件を満たす場合は true。
     */
    bool ProcessMessages() const;

    /**
     * @brief Hwndを取得する関数。
     * @return 戻り値。
     */
    HWND GetHwnd() const;
    /**
     * @brief Widthを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetWidth() const;
    /**
     * @brief Heightを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetHeight() const;
    /**
     * @brief Sizeを取得する関数。
     * @return 戻り値。
     */
    WindowSize GetSize() const;
    /**
     * @brief LastMessageCountを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetLastMessageCount() const;
    /**
     * @brief LastKeyMessageCountを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetLastKeyMessageCount() const;
    /**
     * @brief フレームレートを表示用に反映する関数。
     * @param title 引数。
     */
    void DispFPS(const wchar_t* title);

    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    /**
     * @brief ResizeCallbackを設定する関数。
     * @param callback 引数。
     */
    void SetResizeCallback(ResizeCallback callback);

private:
    /**
     * @brief WndProcを行う関数。
     * @param hwnd Win32 ウィンドウハンドル。
     * @param msg 引数。
     * @param wParam 引数。
     * @param lParam 引数。
     * @return 戻り値。
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    /**
     * @brief HandleMessageを行う関数。
     * @param msg 引数。
     * @param wParam 引数。
     * @param lParam 引数。
     * @return 戻り値。
     */
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

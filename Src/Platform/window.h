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
     * @brief インスタンスの初期状態を整える。
     */
    Window();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~Window();
    /**
     * @brief インスタンスの初期状態を整える。
     */
    Window(const Window&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    Window& operator=(const Window&) = delete;
    /**
     * @brief インスタンスの初期状態を整える。
     */
    Window(Window&&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    Window& operator=(Window&&) = delete;

    /**
     * @brief 内部で使用するリソースを作成する。
     * @param title title に設定する値。
     * @param width 作成または変更後の幅。
     * @param height 作成または変更後の高さ。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool Create(const wchar_t* title, uint32_t width, uint32_t height);
    /**
     * @brief 作成済みウィンドウを表示する。
     */
    void Show() const;
    /**
     * @brief Win32 メッセージを処理し、終了要求の有無を返す。
     * @return 終了要求を受け取っていなければ true。
     */
    bool ProcessMessages() const;

    /**
     * @brief Win32 ウィンドウハンドルを取得する。
     * @return 作成済みウィンドウの HWND。
     */
    HWND GetHwnd() const;
    /**
     * @brief クライアント領域の幅を取得する。
     * @return クライアント領域の幅。
     */
    uint32_t GetWidth() const;
    /**
     * @brief クライアント領域の高さを取得する。
     * @return クライアント領域の高さ。
     */
    uint32_t GetHeight() const;
    /**
     * @brief クライアント領域のサイズを取得する。
     * @return クライアント領域の幅と高さ。
     */
    WindowSize GetSize() const;
    /**
     * @brief 直近に処理したメッセージ数を取得する。
     * @return 直近のメッセージ処理で取り出した全メッセージ数。
     */
    uint32_t GetLastMessageCount() const;
    /**
     * @brief 直近に処理したキー関連メッセージ数を取得する。
     * @return 直近のメッセージ処理で取り出したキー関連メッセージ数。
     */
    uint32_t GetLastKeyMessageCount() const;
    /**
     * @brief タイトルバーへ FPS を表示する。
     * @param title title に設定する値。
     */
    void DispFPS(const wchar_t* title);

    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param callback 通知時に呼び出すコールバック。
     */
    void SetResizeCallback(ResizeCallback callback);

private:
    /**
     * @brief Win32 から届いたメッセージを Window インスタンスへ橋渡しする。
     * @param hwnd 描画対象の Win32 ウィンドウハンドル。
     * @param msg msg に設定する値。
     * @param wParam wParam に設定する値。
     * @param lParam lParam に設定する値。
     * @return Win32 に返すメッセージ処理結果。
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    /**
     * @brief Window インスタンス側で Win32 メッセージを処理する。
     * @param msg msg に設定する値。
     * @param wParam wParam に設定する値。
     * @param lParam lParam に設定する値。
     * @return Win32 に返すメッセージ処理結果。
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

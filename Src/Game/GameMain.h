#pragma once
#include "../Platform/window.h"
#include "../Engine/camera.h"
#include "../Engine/game_instance.h"
#include "../Platform/input.h"
class RenderSystem;

/**
 * @brief GameMainのデータと処理をまとめる型。
 */
class GameMain
{
public:
    /**
     * @brief 値を初期化する。
     */
    GameMain();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~GameMain();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param title ウィンドウや表示に使うタイトル。
     * @param width 作成または変更後の幅。
     * @param height 作成または変更後の高さ。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(const wchar_t* title, int32_t width, int32_t height);
    /**
     * @brief 初期化後にアプリケーションのメインループを回す。
     */
    void Run();
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief 1 フレーム分の状態更新を進める。
     */
    void Tick();
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     */
    void Render();

    /**
     * @brief Window を取得する。
     * @return 保持している Window への参照。
     */
    Window& GetWindow() { return window_; }
    /**
     * @brief 入力状態 を取得する。
     * @return 保持している 入力状態 への参照。
     */
    Input& GetInput() { return input_; }
    /**
     * @brief カメラ を取得する。
     * @return 保持している カメラ への参照。
     */
    Camera& GetCamera() { return camera_; }
    /**
     * @brief Application を取得する。
     * @return 保持している Application への参照。
     */
    GameInstance& GetGameInstance() { return game_instance_; }
    /**
     * @brief RenderSystem を取得する。
     * @return GameMain が保持している RenderSystem。未初期化なら nullptr。
     */
    RenderSystem* GetRenderSystem() const { return render_system_.get(); }
    /**
     * @brief 前フレームからの経過時間を取得する。
     * @return 直前のフレーム更新に使った経過秒数。
     */
    float GetDeltaTime() const { return delta_time_; }
    /**
     * @brief アプリケーションの終了を要求する。                            
     */
    void Quit() { window_.Close(); }

private:
    /**
     * @brief フレーム時間から FPS 表示用の値を更新する。
     */
    void ClacFPS();

    Window window_;
    Input input_;
    Camera camera_;
    std::unique_ptr<RenderSystem> render_system_;
    GameInstance game_instance_;
    float delta_time_ = 0.0f;
};

extern GameMain* game_main;

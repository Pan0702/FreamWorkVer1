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
     * @brief GameMainを初期化するコンストラクタ。
     */
    GameMain();
    /**
     * @brief GameMainの終了処理を行うデストラクタ。
     */
    ~GameMain();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param title 引数。
     * @param width 幅。
     * @param height 高さ。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(const wchar_t* title, int32_t width, int32_t height);
    /**
     * @brief Runを行う関数。
     */
    void Run();
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief Tickを行う関数。
     */
    void Tick();
    /**
     * @brief 登録済みの描画対象を描画する関数。
     */
    void Render();

    /**
     * @brief Windowを取得する関数。
     * @return 戻り値。
     */
    Window& GetWindow() { return window_; }
    /**
     * @brief Inputを取得する関数。
     * @return 戻り値。
     */
    Input& GetInput() { return input_; }
    /**
     * @brief Cameraを取得する関数。
     * @return 戻り値。
     */
    Camera& GetCamera() { return camera_; }
    /**
     * @brief Applicationを取得する関数。
     * @return 戻り値。
     */
    GameInstance& GetApplication() { return game_instance_; }
    /**
     * @brief RenderSystemを取得する関数。
     * @param get 引数。
     * @return 戻り値。
     */
    RenderSystem* GetRenderSystem() const { return render_system_.get(); }
    /**
     * @brief DeltaTimeを取得する関数。
     * @return 戻り値。
     */
    float GetDeltaTime() const { return delta_time_; }
private:
    /**
     * @brief フレームレートを計測し、表示用の値を更新する関数。
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

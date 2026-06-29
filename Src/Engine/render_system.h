#pragma once
#include "../Core/common.h"
#include <memory>
class DebugLineRenderer;
class SceneRenderer;
class Camera;
class Input;
class Actor;
class World;
class DescriptorHeap;
class ConstantBufferAllocator;
class DepthStencil;
class Window;
class GraphicsDevice;
class CommandQueue;
class SwapChain;
class CommandList;

/**
 * @brief RenderSystemのデータと処理をまとめる型。
 */
class RenderSystem
{
public:
    /**
     * @brief 値を初期化する。
     */
    RenderSystem();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~RenderSystem();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param window 描画対象のウィンドウ。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(Window* window);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param world Actor や描画対象を管理する World。
     * @param camera ビュー行列と射影行列を提供するカメラ。
     */
    void Render(World* world, Camera* camera) const;
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief GPU や OS 側へ処理を渡し、必要な同期を進める。
     */
    void WaitForGPU() const;

    /**
     * @brief スワップチェーン を取得する。
     * @return 保持しているスワップチェーン。未作成なら nullptr。
     */
    SwapChain* GetSwapChain() const;
    /**
     * @brief D3D12 コマンドリスト を取得する。
     * @return 保持している D3D12 コマンドリスト。未作成なら nullptr。
     */
    CommandList* GetCommandList() const;
    /**
     * @brief ディスクリプタヒープ を取得する。
     * @return 保持しているディスクリプタヒープ。未作成なら nullptr。
     */
    DescriptorHeap* GetDescriptorHeap() const;
    /**
     * @brief Constant Buffer Allocator を取得する。
     * @return Constant Buffer Allocator。見つからない、または未作成の場合は nullptr。
     */
    ConstantBufferAllocator* GetConstantBufferAllocator() const;
    /**
     * @brief SceneRenderer を取得する。
     * @return SceneRenderer。見つからない、または未作成の場合は nullptr。
     */
    SceneRenderer* GetSceneRenderer() const;
    /**
     * @brief DebugLineRenderer を取得する。
     * @return DebugLineRenderer。見つからない、または未作成の場合は nullptr。
     */
    DebugLineRenderer* GetDebugLineRenderer() const;
    /**
     * @brief Depth Stencil を取得する。
     * @return Depth Stencil。見つからない、または未作成の場合は nullptr。
     */
    DepthStencil* GetDepthStencil() const;
    /**
     * @brief D3D12 デバイス を取得する。
     * @return 保持している D3D12 デバイス。未初期化なら nullptr。
     */
    ID3D12Device* GetDevice() const;
    /**
     * @brief 直近の Present 計測時間を取得する。
     * @return 直近の Present にかかったミリ秒単位の時間。
     */
    float GetLastPresentMs() const { return last_present_ms_; }
    /**
     * @brief 直近の GPU 待機時間を取得する。
     * @return 直近の GPU 完了待ちにかかったミリ秒単位の時間。
     */
    float GetLastGpuWaitMs() const { return last_gpu_wait_ms_; }


private:
    std::unique_ptr<GraphicsDevice> graphics_device_;
    std::unique_ptr<CommandQueue> command_queue_;
    std::unique_ptr<SwapChain> swap_chain_;
    std::unique_ptr<CommandList> command_list_;
    std::unique_ptr<DepthStencil> depth_stencil_;
    std::unique_ptr<DescriptorHeap> srv_heap_;
    std::unique_ptr<ConstantBufferAllocator> cb_allocator_;
    std::unique_ptr<SceneRenderer> scene_renderer_;

    Window* window_ = nullptr;
    mutable float last_present_ms_ = 0.0f;
    mutable float last_gpu_wait_ms_ = 0.0f;
};

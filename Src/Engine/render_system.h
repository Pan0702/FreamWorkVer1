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
     * @brief RenderSystemを初期化するコンストラクタ。
     */
    RenderSystem();
    /**
     * @brief RenderSystemの終了処理を行うデストラクタ。
     */
    ~RenderSystem();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param window 描画先または入力元になるウィンドウ。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(Window* window);
    /**
     * @brief 登録済みの描画対象を描画する関数。
     * @param world 引数。
     * @param camera 引数。
     */
    void Render(World* world, Camera* camera) const;
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief WaitForGPUを行う関数。
     */
    void WaitForGPU() const;

    /**
     * @brief SwapChainを取得する関数。
     * @return 戻り値。
     */
    SwapChain* GetSwapChain() const;
    /**
     * @brief CommandListを取得する関数。
     * @return 戻り値。
     */
    CommandList* GetCommandList() const;
    /**
     * @brief DescriptorHeapを取得する関数。
     * @return 戻り値。
     */
    DescriptorHeap* GetDescriptorHeap() const;
    /**
     * @brief ConstantBufferAllocatorを取得する関数。
     * @return 戻り値。
     */
    ConstantBufferAllocator* GetConstantBufferAllocator() const;
    /**
     * @brief SceneRendererを取得する関数。
     * @return 戻り値。
     */
    SceneRenderer* GetSceneRenderer() const;
    /**
     * @brief DebugLineRendererを取得する関数。
     * @return 戻り値。
     */
    DebugLineRenderer* GetDebugLineRenderer() const;
    /**
     * @brief DepthStencilを取得する関数。
     * @return 戻り値。
     */
    DepthStencil* GetDepthStencil() const;
    /**
     * @brief Deviceを取得する関数。
     * @return 戻り値。
     */
    ID3D12Device* GetDevice() const;
    /**
     * @brief LastPresentMsを取得する関数。
     * @return 戻り値。
     */
    float GetLastPresentMs() const { return last_present_ms_; }
    /**
     * @brief 前回のグラフィックス処理装置待機時間をミリ秒で取得する関数。
     * @return 戻り値。
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

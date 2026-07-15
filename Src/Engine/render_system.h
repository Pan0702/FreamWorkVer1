#pragma once
#include "../Core/common.h"
#include <memory>

#include "../Core/Math/my_math.h"
#include "../Graphics/graphics_config.h"
#include "../Graphics/constant_buffer_allocator.h"
class DebugLineRenderer;
class SceneRenderer;
class Camera;
class Input;
class Actor;
class World;
class DescriptorHeap;
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
     */
    void Render();
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

private:
    static constexpr uint32 kFrameResource = kFrameCount;

    struct FrameResource
    {
        ComPtr<ID3D12CommandAllocator> command_allocator;
        ConstantBufferAllocator cb_allocator;
        uint64 fence_value = 0;
    /**
     * @brief フレームごとの定数バッファを初期化する。
     */
        bool Initialize(ID3D12Device* device, uint32 cb_size);
    };


    std::unique_ptr<GraphicsDevice> graphics_device_;
    std::unique_ptr<CommandQueue> command_queue_;
    std::unique_ptr<SwapChain> swap_chain_;
    std::unique_ptr<CommandList> command_list_;
    std::unique_ptr<DepthStencil> depth_stencil_;
    std::unique_ptr<DescriptorHeap> srv_heap_;
    std::unique_ptr<SceneRenderer> scene_renderer_;

    Window* window_ = nullptr;
    FrameResource frames_[kFrameResource];
    uint64 frame_count_ = 0;
};

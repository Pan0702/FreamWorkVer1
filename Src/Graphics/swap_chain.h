#pragma once
#include "../Core/common.h"
#include  "descriptor_heap.h"
#include "graphics_config.h"

/**
 * @brief SwapChainのデータと処理をまとめる型。
 */
class SwapChain
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param factory SwapChain 作成に使用する DXGI ファクトリ。
     * @param device 使用する D3D12 デバイス。
     * @param queue GPU コマンドを実行するキュー。
     * @param hwnd 描画対象の Win32 ウィンドウハンドル。
     * @param width 作成または変更後の幅。
     * @param height 作成または変更後の高さ。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(IDXGIFactory6* factory, ID3D12Device* device, ID3D12CommandQueue* queue,
                    HWND hwnd, uint32_t width, uint32_t height);
    /**
     * @brief GPU や OS 側へ処理を渡し、必要な同期を進める。
     */
    void Present();
    /**
     * @brief Current Back Buffer を取得する。
     * @return Current Back Buffer。見つからない、または未作成の場合は nullptr。
     */
    ID3D12Resource* GetCurrentBackBuffer() const;
    /**
     * @brief Current Rtv Handle を取得する。
     * @return 現在保持している Current Rtv Handle。
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const;
    /**
     * @brief サイズ変更に合わせて GPU リソースを作り直す。
     * @param width 作成または変更後の幅。
     * @param height 作成または変更後の高さ。
     * @return 新しいサイズでリソースを作り直せた場合は true。
     */
    bool Resize(uint32_t width, uint32_t height);

private:
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param factory SwapChain 作成に使用する DXGI ファクトリ。
     * @param command_queue GPU コマンドを実行するキュー。
     * @param hwnd 描画対象の Win32 ウィンドウハンドル。
     * @param width 作成または変更後の幅。
     * @param height 作成または変更後の高さ。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateSwapChain(IDXGIFactory6* factory, ID3D12CommandQueue* command_queue,
                         HWND hwnd, uint32_t width, uint32_t height);

    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateRenderTargetViews(ID3D12Device* device);

    ComPtr<IDXGISwapChain4> swap_chain_;
    DescriptorHeap rtv_heap_ = {};
    ID3D12Device* device_ = nullptr;
    ComPtr<ID3D12Resource> render_targets_[kFrameCount];
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles_[kFrameCount] = {};
};

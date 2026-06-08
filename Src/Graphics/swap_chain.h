#pragma once
#include "../Core/common.h"
#include "../Platform/Window.h"
#include  "descriptor_heap.h"

/**
 * @brief SwapChainのデータと処理をまとめる型。
 */
class SwapChain
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param factory 引数。
     * @param device DirectX 12 デバイス。
     * @param queue コマンド実行に使うキュー。
     * @param hwnd Win32 ウィンドウハンドル。
     * @param width 幅。
     * @param height 高さ。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(IDXGIFactory6* factory, ID3D12Device* device, ID3D12CommandQueue* queue,
                    HWND hwnd, uint32_t width, uint32_t height);
    /**
     * @brief バックバッファを画面へ表示する関数。
     */
    void Present();
    /**
     * @brief CurrentBackBufferを取得する関数。
     * @return 戻り値。
     */
    ID3D12Resource* GetCurrentBackBuffer() const;
    /**
     * @brief 現在のバックバッファ用レンダーターゲットビューハンドルを取得する関数。
     * @return 戻り値。
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle() const;
    /**
     * @brief Resizeを行う関数。
     * @param width 幅。
     * @param height 高さ。
     * @return 条件を満たす場合は true。
     */
    bool Resize(uint32_t width, uint32_t height);

private:
    /**
     * @brief CreateSwapChainを行う関数。
     * @param factory 引数。
     * @param command_queue コマンド実行に使うキュー。
     * @param hwnd Win32 ウィンドウハンドル。
     * @param width 幅。
     * @param height 高さ。
     * @return 条件を満たす場合は true。
     */
    bool CreateSwapChain(IDXGIFactory6* factory, ID3D12CommandQueue* command_queue,
                         HWND hwnd, uint32_t width, uint32_t height);

    /**
     * @brief CreateRenderTargetViewsを行う関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool CreateRenderTargetViews(ID3D12Device* device);

    ComPtr<IDXGISwapChain4> swap_chain_;
    DescriptorHeap rtv_heap_ = {};
    ID3D12Device* device_ = nullptr;
    ComPtr<ID3D12Resource> render_targets_[kFrameCount];
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles_[kFrameCount] = {};
};

#pragma once
#include "descriptor_heap.h"
#include "../Core/common.h"

/**
 * @brief DepthStencilのデータと処理をまとめる型。
 */
class DepthStencil
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param width 幅。
     * @param height 高さ。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, uint32_t width, uint32_t height);
    /**
     * @brief 中央処理装置側ディスクリプタハンドルを取得する関数。
     * @return 戻り値。
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;
    static constexpr DXGI_FORMAT kFORMAT = DXGI_FORMAT_D32_FLOAT;
    /**
     * @brief Resizeを行う関数。
     * @param w 引数。
     * @param h 引数。
     * @return 条件を満たす場合は true。
     */
    bool Resize(uint32_t w, uint32_t h);

private:
    /**
     * @brief CreateResourceを行う関数。
     * @param w 引数。
     * @param h 引数。
     * @return 条件を満たす場合は true。
     */
    bool CreateResource(uint32_t w, uint32_t h);

    ComPtr<ID3D12Resource> depth_stencil_;
    DescriptorHeap heap_;
    ID3D12Device* device_ = nullptr;
};

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
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param width 作成または変更後の幅。
     * @param height 作成または変更後の高さ。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, uint32_t width, uint32_t height);
    /**
     * @brief CPU ディスクリプタハンドル を取得する。
     * @return 現在保持している CPU ディスクリプタハンドル。
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;
    static constexpr DXGI_FORMAT kFORMAT = DXGI_FORMAT_D32_FLOAT;
    /**
     * @brief サイズ変更に合わせて GPU リソースを作り直す。
     * @param w W 成分。
     * @param h 処理対象の値。
     * @return 新しいサイズでリソースを作り直せた場合は true。
     */
    bool Resize(uint32_t w, uint32_t h);

private:
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param w W 成分。
     * @param h 処理対象の値。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateResource(uint32_t w, uint32_t h);

    ComPtr<ID3D12Resource> depth_stencil_;
    DescriptorHeap heap_;
    ID3D12Device* device_ = nullptr;
};

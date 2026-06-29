#pragma once
#include "../Core/common.h"

/**
 * @brief ConstantBufferのデータと処理をまとめる型。
 */
class ConstantBuffer
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param size 設定するサイズ。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, uint32_t size);
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param data 転送するデータの先頭アドレス。
     * @param size 設定するサイズ。
     */
    void Update(const void* data, uint32_t size);
    /**
     * @brief Gpu Address を取得する。
     * @return 現在保持している Gpu Address。
     */
    D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    void* mapped_ = nullptr;
    uint32_t aligned_size_ = 0;
};

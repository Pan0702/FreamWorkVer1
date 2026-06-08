#pragma once
#include "../Core/common.h"

/**
 * @brief ConstantBufferのデータと処理をまとめる型。
 */
class ConstantBuffer
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param size サイズ情報。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, uint32_t size);
    /**
     * @brief 1 フレーム分の状態を更新する関数。
     * @param data コピー元データ。
     * @param size サイズ情報。
     */
    void Update(const void* data, uint32_t size);
    /**
     * @brief 定数バッファのグラフィックス処理装置側アドレスを取得する関数。
     * @return 戻り値。
     */
    D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    void* mapped_ = nullptr;
    uint32_t aligned_size_ = 0;
};

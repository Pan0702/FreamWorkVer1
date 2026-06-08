#pragma once
#include "../Core/common.h"

/**
 * @brief ConstantBufferAllocationのデータと処理をまとめる型。
 */
struct ConstantBufferAllocation
{
    void* cpu = nullptr;                       // CPU から書き込む先頭アドレス。
    D3D12_GPU_VIRTUAL_ADDRESS gpu = 0;         // シェーダーへ渡す GPU 仮想アドレス。
    uint32_t size = 0;                         // 確保したバイト数。
};

/**
 * @brief ConstantBufferAllocatorのデータと処理をまとめる型。
 */
class ConstantBufferAllocator
{
public:
    ConstantBufferAllocator() = default;
    /**
     * @brief ConstantBufferAllocatorの終了処理を行うデストラクタ。
     */
    ~ConstantBufferAllocator();

    ConstantBufferAllocator(const ConstantBufferAllocator&) = delete;
    ConstantBufferAllocator& operator=(const ConstantBufferAllocator&) = delete;

    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param capacity 引数。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, uint32_t capacity);
    /**
     * @brief 定数バッファから指定サイズの領域を割り当てる関数。
     * @param size サイズ情報。
     * @param out_allocation 引数。
     * @return 条件を満たす場合は true。
     */
    bool Allocate(uint32_t size, ConstantBufferAllocation* out_allocation);
    /**
     * @brief 再利用できるように状態をリセットする関数。
     */
    void Reset();

    /**
     * @brief Capacityを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetCapacity() const;
    /**
     * @brief UsedSizeを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetUsedSize() const;

private:
    /**
     * @brief AlignConstantBufferSizeを行う関数。
     * @param size サイズ情報。
     * @return 戻り値。
     */
    static uint32_t AlignConstantBufferSize(uint32_t size);

    ComPtr<ID3D12Resource> buffer_;
    void* mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_base_ = 0;
    uint32_t capacity_ = 0;
    uint32_t offset_ = 0;
};

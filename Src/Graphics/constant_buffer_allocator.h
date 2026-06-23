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
    /**
     * @brief インスタンスの初期状態を整える。
     */
    ConstantBufferAllocator() = default;
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~ConstantBufferAllocator();

    /**
     * @brief インスタンスの初期状態を整える。
     */
    ConstantBufferAllocator(const ConstantBufferAllocator&) = delete;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    ConstantBufferAllocator& operator=(const ConstantBufferAllocator&) = delete;

    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param capacity capacity に設定する値。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, uint32_t capacity);
    /**
     * @brief アラインメントを考慮してバッファ領域を切り出す。
     * @param size size に設定する値。
     * @param out_allocation out_allocation に設定する値。
     * @return アラインメントを考慮してバッファ領域を切り出す 場合は true。
     */
    bool Allocate(uint32_t size, ConstantBufferAllocation* out_allocation);
    /**
     * @brief 再利用できるように内部状態を初期位置へ戻す。
     */
    void Reset();

    /**
     * @brief 定数バッファ領域の容量を取得する。
     * @return 確保済み定数バッファ領域のバイト数。
     */
    uint32_t GetCapacity() const;
    /**
     * @brief 使用済み定数バッファ領域のサイズを取得する。
     * @return 現在割り当て済みの定数バッファ領域のバイト数。
     */
    uint32_t GetUsedSize() const;

private:
    /**
     * @brief Constant Buffer の 256 バイト境界にサイズを丸める。
     * @param size size に設定する値。
     * @return 256 バイト境界に切り上げたサイズ。
     */
    static uint32_t AlignConstantBufferSize(uint32_t size);

    ComPtr<ID3D12Resource> buffer_;
    void* mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_base_ = 0;
    uint32_t capacity_ = 0;
    uint32_t offset_ = 0;
};

#pragma once
#include "../Core/common.h"

/**
 * @brief DescriptorHeapのデータと処理をまとめる型。
 */
class DescriptorHeap
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param type 引数。
     * @param capacity 引数。
     * @param shader_visible 引数。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity, bool shader_visible);
    /**
     * @brief 中央処理装置側ディスクリプタハンドルを取得する関数。
     * @param index 参照する番号。
     * @return 戻り値。
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const;
    /**
     * @brief グラフィックス処理装置側ディスクリプタハンドルを取得する関数。
     * @param index 参照する番号。
     * @return 戻り値。
     */
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const;
    /**
     * @brief Heapを取得する関数。
     * @return 戻り値。
     */
    ID3D12DescriptorHeap* GetHeap() const;
    /**
     * @brief ディスクリプタヒープから 1 つのディスクリプタ番号を割り当てる関数。
     * @param out_index 引数。
     * @return 条件を満たす場合は true。
     */
    bool Allocate(UINT& out_index);

private:
    ComPtr<ID3D12DescriptorHeap> heap_;
    D3D12_DESCRIPTOR_HEAP_TYPE type_;
    UINT descriptor_size_;
    UINT capacity_;
    UINT used_count_;
    bool shader_visible_;
};

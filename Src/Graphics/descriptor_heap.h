#pragma once
#include "../Core/common.h"

/**
 * @brief DescriptorHeapのデータと処理をまとめる型。
 */
class DescriptorHeap
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param type type に設定する値。
     * @param capacity capacity に設定する値。
     * @param shader_visible shader_visible に設定する値。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT capacity, bool shader_visible);
    /**
     * @brief CPU ディスクリプタハンドル を取得する。
     * @param index index に設定する値。
     * @return 現在保持している CPU ディスクリプタハンドル。
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(UINT index) const;
    /**
     * @brief GPU ディスクリプタハンドル を取得する。
     * @param index index に設定する値。
     * @return 現在保持している GPU ディスクリプタハンドル。
     */
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT index) const;
    /**
     * @brief Heap を取得する。
     * @return Heap。見つからない、または未作成の場合は nullptr。
     */
    ID3D12DescriptorHeap* GetHeap() const;
    /**
     * @brief アラインメントを考慮してバッファ領域を切り出す。
     * @param out_index out_index に設定する値。
     * @return アラインメントを考慮してバッファ領域を切り出す 場合は true。
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

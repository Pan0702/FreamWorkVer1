#pragma once
#include <cstdint>
#include "../Core/common.h"

/**
 * @brief IndexBufferのデータと処理をまとめる型。
 */
class IndexBuffer
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param index_data index_data に設定する値。
     * @param total_size total_size に設定する値。
     * @param format format に設定する値。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, const void* index_data, uint32_t total_size, DXGI_FORMAT format);
    /**
     * @brief インデックスバッファビュー を取得する。
     * @return 現在保持している インデックスバッファビュー。
     */
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
    /**
     * @brief インデックス数を取得する。
     * @return 描画に使うインデックス数。
     */
    uint32_t GetIndexCount() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    D3D12_INDEX_BUFFER_VIEW view_ = {};
    uint32_t index_count_ = 0;
};

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
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param index_data インデックスバッファ作成に使うデータ。
     * @param total_size 引数。
     * @param format 出力する書式文字列。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, const void* index_data, uint32_t total_size, DXGI_FORMAT format);
    /**
     * @brief IndexBufferViewを取得する関数。
     * @return 戻り値。
     */
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
    /**
     * @brief IndexCountを取得する関数。
     * @return 戻り値。
     */
    uint32_t GetIndexCount() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    D3D12_INDEX_BUFFER_VIEW view_ = {};
    uint32_t index_count_ = 0;
};

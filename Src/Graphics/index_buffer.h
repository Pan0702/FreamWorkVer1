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
     * @param index_data アップロードするインデックスデータ。
     * @param total_size アップロードするデータ全体のバイト数。
     * @param format 表示またはリソース作成に使うフォーマット。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    /**
     * @brief インデックスデータから GPU 用バッファを初期化する。
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

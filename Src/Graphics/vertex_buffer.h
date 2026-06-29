#pragma once
#include "../Core/common.h"

/**
 * @brief VertexBufferのデータと処理をまとめる型。
 */
class VertexBuffer
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param vertex_data アップロードする頂点データ。
     * @param total_size アップロードするデータ全体のバイト数。
     * @param stride 1 頂点あたりのバイト数。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, const void* vertex_data
                    , uint32_t total_size, uint32_t stride);

    /**
     * @brief 頂点バッファビュー を取得する。
     * @return 現在保持している 頂点バッファビュー。
     */
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    D3D12_VERTEX_BUFFER_VIEW view_ = {};
};

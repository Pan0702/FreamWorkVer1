#pragma once
#include "../Core/common.h"

/**
 * @brief VertexBufferのデータと処理をまとめる型。
 */
class VertexBuffer
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param vertex_data 頂点バッファ作成に使うデータ。
     * @param total_size 引数。
     * @param stride 引数。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, const void* vertex_data
                    , uint32_t total_size, uint32_t stride);

    /**
     * @brief VertexBufferViewを取得する関数。
     * @return 戻り値。
     */
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
    ComPtr<ID3D12Resource> buffer_;
    D3D12_VERTEX_BUFFER_VIEW view_ = {};
};

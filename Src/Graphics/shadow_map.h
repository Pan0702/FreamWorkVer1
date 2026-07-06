#pragma once
#include "descriptor_heap.h"
#include "graphics_device.h"
#include "../Core/Math/my_math.h"

/**
 * @brief シャドウマップ用の深度テクスチャとそれを参照するためのDSV/SRV を管理するクラス
 */
class ShadowMap
{
public:
    /**
     * @brief 車道マップ用の深度テクスチャとディスクリプタを作成
     * @param device リソース作成に使うdevice
     * @param size シャドウマップの幅と高さ。
     * @param srv_heap 車道マップをシェーダーから読み込むためのSRVを確保するヒープ
     * @return 初期化に成功したらTrue
     */
    bool Initialize(ID3D12Device* device,uint32 size,DescriptorHeap* srv_heap);

    /**
     * @brief　深度書き込み用のDSVハンドルを取得する。
     * @return DSVにCPUハンドル
     */
    D3D12_CPU_DESCRIPTOR_HANDLE GetDRV() const;

    /**
     * @brief　SRVヒープ上に確保したシャドウマップのインデックスを取得する。
     * @return SRVとしてバインドするためのひーぷいんでっくす。
     */
    uint32 GetSrvIndex() const;

    /**
     * @brief　車道マップ本体のD3D12リソースを取得する。
     * @return 深度テクスチャのリソースポインタ
     */
    ID3D12Resource* GetResource() const;
    
    /**
     * シャドウマップの1辺のサイズを取得する。
     * @return シャドウマップのの幅と高さ
     */
    uint32 GetSize() const;
private:
    /**
     * @brief 初期化時に確保した情報を使って深度テクスチャとDSV/SRVを作成する
     * @return 成功したらTrueを返す
     */
    bool Create();
    ComPtr<ID3D12Resource> texture_;
    ID3D12Device* device_ = nullptr;
    DescriptorHeap* srv_heap_ = nullptr;
    DescriptorHeap dsv_heap_ = {};
    uint32 srv_index_ = 0;
    uint32 size_ = 0;
};

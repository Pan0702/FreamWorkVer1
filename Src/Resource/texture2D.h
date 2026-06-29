#pragma once
#include "../Core/common.h"
#include "texture_loder.h"
/**
 * @brief Texture2Dのデータと処理をまとめる型。
 */
class Texture2D
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param cmd_list テクスチャアップロードに使うコマンドリスト。
     * @param image アップロードまたは解放する画像データ。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const LoadedImage& image);
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @param srv_handle SRV を配置する CPU ディスクリプタ。
     */
    void CreateSrv(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srv_handle);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param srv_index SRV ヒープ上のインデックス。
     */
    void SetSrvIndex(uint32_t srv_index);
    /**
     * @brief SRV ヒープ上のインデックスを取得する。
     * @return テクスチャ SRV が配置されているヒープ内インデックス。
     */
    uint32_t GetSrvIndex() const;

private:
    ComPtr<ID3D12Resource> texture_;
    ComPtr<ID3D12Resource> upload_buffer_;
    uint32_t srv_index_ = 0;
};

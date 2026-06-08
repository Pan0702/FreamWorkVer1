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
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param cmd_list DirectX 12 コマンドリスト。
     * @param image テクスチャ化する読み込み済み画像。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const LoadedImage& image);
    /**
     * @brief テクスチャ用のシェーダーリソースビューを作成する関数。
     * @param device DirectX 12 デバイス。
     * @param srv_handle 引数。
     */
    void CreateSrv(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srv_handle);
    /**
     * @brief シェーダーリソースビューのディスクリプタ番号を設定する関数。
     * @param srv_index 引数。
     */
    void SetSrvIndex(uint32_t srv_index);
    /**
     * @brief シェーダーリソースビューのディスクリプタ番号を取得する関数。
     * @return 戻り値。
     */
    uint32_t GetSrvIndex() const;

private:
    ComPtr<ID3D12Resource> texture_;
    ComPtr<ID3D12Resource> upload_buffer_;
    uint32_t srv_index_ = 0;
};
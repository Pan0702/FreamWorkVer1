#pragma once
#include "../Core/common.h"
#include "texture_loder.h"

class Texture2D
{
public:
    // image を GPU テクスチャ化し、アップロードコマンドを cmd_list に「記録」する。
    // コマンドリストの実行・GPU同期は呼び出し側（TextureManager）の責務。
    bool Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const LoadedImage& image);

    void CreateSrv(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srv_handle);
    void SetSrvIndex(uint32_t srv_index);
    uint32_t GetSrvIndex() const;

private:
    ComPtr<ID3D12Resource> texture_;
    ComPtr<ID3D12Resource> upload_buffer_;
    uint32_t srv_index_ = 0;
};
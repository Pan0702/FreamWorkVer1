#pragma once
#include "../Core/common.h"
#include "texture_loder.h"
class Texture2D
{
public:
    bool Initialize(ID3D12Device* device,ID3D12CommandQueue* queue,
        ID3D12GraphicsCommandList* cmd_list,ID3D12CommandAllocator* allocator,
        const LoadedImage& image);
    
    void CreateSrv(ID3D12Device* device,D3D12_CPU_DESCRIPTOR_HANDLE srv_handle);
    
private:
    ComPtr<ID3D12Resource> texture_;
    ComPtr<ID3D12Resource> upload_buffer_;
};

#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/descriptor_heap.h"
#include "../Resource/material.h"

class ConstantBufferAllocator;
class DescriptorHeap;
class PipelineStateCache;
/**
 * @brief RenderContextのデータと処理をまとめる型。
 */
struct RenderContext
{
    ID3D12GraphicsCommandList* command_list = nullptr;
    DescriptorHeap* srv_heap = nullptr;
    ConstantBufferAllocator* cb_allocator = nullptr;
    PipelineStateCache* pso_cache = nullptr;
    Vec2 screen_size = {0.0f, 0.0f};
    D3D12_GPU_VIRTUAL_ADDRESS light_cb_address = 0;
    // シャドウマップを通常描画で参照するための SRVヒープインデックス
    uint32 shadow_srv_index = 0;
    uint32 irradiance_srv_index = 0;
};

inline void BindMaterialTexture(ID3D12GraphicsCommandList* c,const DescriptorHeap* heap,
    const MaterialBinding& b,UINT diffuse,UINT normal,UINT specular,UINT height)
{
    c->SetGraphicsRootDescriptorTable(diffuse, heap->GetGpuHandle(b.diffuse_srv));
    c->SetGraphicsRootDescriptorTable(normal, heap->GetGpuHandle(b.normal_srv));
    c->SetGraphicsRootDescriptorTable(specular, heap->GetGpuHandle(b.specular_srv));
    c->SetGraphicsRootDescriptorTable(height, heap->GetGpuHandle(b.height_srv));
}

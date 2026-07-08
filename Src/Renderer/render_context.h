#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"

class ConstantBufferAllocator;
class DescriptorHeap;
/**
 * @brief RenderContextのデータと処理をまとめる型。
 */
struct RenderContext
{
    ID3D12GraphicsCommandList* command_list = nullptr;
    DescriptorHeap* srv_heap = nullptr;
    ConstantBufferAllocator* cb_allocator = nullptr;
    Vec2 screen_size = {0.0f, 0.0f};
    // シャドウマップを通常描画で参照するための SRVヒープインデックス
    uint32 shadow_srv_index = 0;
    uint32 irradiance_srv_index = 0;
    
};

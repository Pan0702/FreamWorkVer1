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
    Mat view = Identity();
    Mat projection = Identity();
    DescriptorHeap* srv_heap = nullptr;
    ConstantBufferAllocator* cb_allocator = nullptr;
    Vec2 screen_size = {0.0f, 0.0f};
    
    //光の向き//
    Vec3 light_pos = Vec3(0,0,0);
    // 光の強さ//
    Vec3 light_color = Vec3(1.0f,1.0f,1.0f);
    Vec3 sky_color = Vec3(0.5f,0.7f,1.0f);
    Vec3 ground_color = Vec3(0.2f,0.3f,0.3f);
    Vec3 camera_pos = Vec3(0.0f,0.0f,0.0f);
};

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
    
    Vec3 light_dir = Vec3(0.3f,-1.0f,0.5f); 
    Vec3 light_color = Vec3(1.0f,1.0f,1.0f); 
    Vec3 ambient = Vec3(0.2f,0.2f,0.2f); 
    Vec3 camera_pos = Vec3(0.0f,0.0f,0.0f);
};

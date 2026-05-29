#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"

class ConstantBufferAllocator;
class DescriptorHeap;

struct RenderContext
{
    ID3D12GraphicsCommandList* command_list = nullptr;
    Mat view = Identity();
    Mat projection = Identity();
    DescriptorHeap* srv_heap = nullptr;
    ConstantBufferAllocator* cb_allocator = nullptr;
    Vec2 screen_size = {0.0f, 0.0f};
};

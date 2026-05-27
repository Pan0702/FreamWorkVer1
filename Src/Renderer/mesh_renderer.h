#pragma once
#include <memory>
#include <vector>

#include "../Core/common.h"
#include "../Graphics/constant_buffer.h"

class DescriptorHeap;
class Camera;
class RenderObject;

class MeshRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
                Camera* camera,DescriptorHeap* descriptor_heap);
private:
    std::unique_ptr<ConstantBuffer> constant_buffer_;
};

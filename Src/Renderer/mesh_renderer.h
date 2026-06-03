#pragma once
#include <vector>

#include "../Core/common.h"
#include "draw_command.h"

class Camera;
class DescriptorHeap;
struct RenderContext;
class RenderObject;
class StaticMeshComponent;

class MeshRenderer
{
public:
    bool Initialize(const ID3D12Device* device);
    void Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
                Camera* camera, DescriptorHeap* descriptor_heap);
    void Register(StaticMeshComponent* component);
    void Unregister(StaticMeshComponent* component);
    void Collect();
    void Sort();
    void Submit(RenderContext& context);

private:
    std::vector<StaticMeshComponent*> registered_;
    std::vector<DrawCommand> draw_commands_;
};

#pragma once
#include <vector>
#include "../Core/common.h"
#include "draw_command.h"

class PipelineState;
class RootSignature;
struct RenderContext;
class SkeletalMeshComponent;
class Camera;
class DescriptorHeap;
class RenderObject;

class SkinnedMeshRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
                Camera* camera, DescriptorHeap* descriptor_heap);
    void Register(SkeletalMeshComponent* component);
    void Unregister(SkeletalMeshComponent* component);
    void Collect();
    void Sort();
    void Submit(RenderContext& context);

private:
    std::vector<SkeletalMeshComponent*> meshes_;
    std::vector<SkinnedDrawCommand> draw_commands_;
    std::unique_ptr<RootSignature> root_signature_;   // Å© í«â¡
    std::unique_ptr<PipelineState> pipeline_state_;   // Å© í«â¡
};

#pragma once
#include "render_context.h"
#include "../Graphics/command_list.h"
#include "../Graphics/shadow_map.h"

class SkinnedMeshRenderer;
class MeshRenderer;
class PipelineState;
class Shader;
class RootSignature;

class ShadowRenderer
{
public:
    bool Initialize(ID3D12Device* device, DescriptorHeap* srv_heap);
    void RenderShadowPass(RenderContext& context, const MeshRenderer* mesh, const SkinnedMeshRenderer* skinned) const;
    uint32 GetShadowMapIndex() const;
private:
    std::unique_ptr<RootSignature> shadow_root_signature_; 
    std::unique_ptr<Shader> shadow_vs_;
    std::unique_ptr<PipelineState> shadow_pso_;
    
    std::unique_ptr<RootSignature> shadow_sk_root_signature_; 
    std::unique_ptr<Shader> shadow_sk_vs_;
    std::unique_ptr<PipelineState> shadow_sk_pso_;
    ShadowMap shadow_map_;
};

#pragma once
#include <memory>
#include "../Core/common.h"

struct RenderContext;
class Texture2D;
class PipelineState;
class RootSignature;
class Mesh;

class SkyRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void SetTexture(Texture2D* texture);
    void Render(const RenderContext& context) const;
private:
    std::unique_ptr<Mesh> sky_mesh_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;   
    Texture2D* sky_texture_ = nullptr;  
};

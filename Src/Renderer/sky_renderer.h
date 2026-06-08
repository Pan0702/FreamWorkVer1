#pragma once
#include <memory>
#include "../Core/common.h"

class Texture2D;
class PipelineState;
class RootSignature;
class Mesh;

class sky_renderer
{
public:
    bool Initialize(ID3D12Device* device);
    void SetTexture();
    void Render();
private:
    std::unique_ptr<Mesh> sky_mesh_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;   
    Texture2D* sky_texture_ = nullptr;  
};

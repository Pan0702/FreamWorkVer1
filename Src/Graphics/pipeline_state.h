#pragma once
#include "../Core/common.h"
class Shader;
class PipelineState
{
public:
    bool Initialize(ID3D12Device* device, ID3D12RootSignature* root_signature
                    , const Shader& vertex_shader, const Shader& pixel_shader);
    
    ID3D12PipelineState* GetPipelineState() const;
    
private:  
    ComPtr<ID3D12PipelineState> pipeline_state_;
    
};

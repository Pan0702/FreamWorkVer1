#include "material.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/shader.h"
#include "../Resource/texture2D.h"
bool Material::Create(ID3D12Device* device, const wchar_t* vs_path, const wchar_t* ps_path,
    std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout)
{
    
    root_signature_ = std::make_unique<RootSignature>();
    if
    (!root_signature_->Initialize(device))
    {
        MessageBox(nullptr, L"Failed to create root signature", L"Error", MB_OK);
        return false;
    }

    vertex_shader_ = std::make_unique<Shader>();
    if (!vertex_shader_->LoadFromFile(vs_path, "VSMain", "vs_5_0"))
    {
        MessageBox(nullptr, L"Failed to load vertex shader", L"Error", MB_OK);
        return false;
    }

    pixel_shader_ = std::make_unique<Shader>();
    if (!pixel_shader_->LoadFromFile(ps_path, "PSMain", "ps_5_0"))
    {
        MessageBox(nullptr, L"Failed to load pixel shader", L"Error", MB_OK);
        return false;
    }

    pipeline_state_ = std::make_unique<PipelineState>();
    if (!pipeline_state_->Initialize(device, root_signature_->GetRootSignature(),
                                     *vertex_shader_, *pixel_shader_, input_layout))
    {
        MessageBox(nullptr, L"Failed to create pipeline state", L"Error", MB_OK);
        return false;
    }
    return true;
}

void Material::Apply(ID3D12GraphicsCommandList* command_list)
{
    command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
}

void Material::SetDiffuse(Texture2D* diffuse)
{
    diffuse_ = diffuse;
}

void Material::SetNormal(Texture2D* normal)
{
    normal_ = normal;
}

void Material::SetSpecular(Texture2D* specular)
{
    specular_ = specular;
}

void Material::SetHeight(Texture2D* height)
{
    height_ = height;   
}

Texture2D* Material::GetDiffuse() const
{
    return diffuse_;
}

Texture2D* Material::GetNormal() const
{
    return normal_;  
}

Texture2D* Material::GetSpecular() const
{
    return specular_; 
}

Texture2D* Material::GetHeight() const
{
    return height_; 
}

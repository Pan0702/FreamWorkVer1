#pragma once
#include <cstdint>
#include <memory>
#include <span>

#include "../Graphics/graphics_device.h"

class Texture2D;
class PipelineState;
class RootSignature;
class Shader;

class Material
{
public:
    bool Create(ID3D12Device* device,const wchar_t* vs_path,const wchar_t* ps_path,std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout);
    void Apply(ID3D12GraphicsCommandList* command_list);
    void SetDiffuse(Texture2D* diffuse);
    void SetNormal(Texture2D* normal);
    void SetSpecular(Texture2D* specular);
    void SetHeight(Texture2D* height);
    Texture2D* GetDiffuse() const;
    Texture2D* GetNormal() const;
    Texture2D* GetSpecular() const;
    Texture2D* GetHeight() const;
private:
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader>  pixel_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    Texture2D* diffuse_;
    Texture2D* normal_;
    Texture2D* specular_;
    Texture2D* height_;
};

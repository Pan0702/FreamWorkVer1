#pragma once
#include <cstdint>
#include <memory>
#include <span>

#include "mesh.h"
#include "../Core/Math/vec4.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include  "../Graphics/shader.h"
class DescriptorHeap;
class Texture2D;


class Material
{
public:
    Material();
    Material(const MeshMaterialDesc& desc);
    bool Create(ID3D12Device* device, const wchar_t* vs_path, const wchar_t* ps_path,
                std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout);
    void Apply(ID3D12GraphicsCommandList* command_list, DescriptorHeap* descriptor_heap);
    void SetDiffuse(Texture2D* diffuse);
    void SetNormal(Texture2D* normal);
    void SetSpecular(Texture2D* specular);
    void SetHeight(Texture2D* height);
    void SetBaseColor(const Vec4& color);
    Texture2D* GetDiffuse() const;
    Texture2D* GetNormal() const;
    Texture2D* GetSpecular() const;
    Texture2D* GetHeight() const;
    Vec4 GetBaseColor() const;

private:
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader> pixel_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    Texture2D* diffuse_ = nullptr;
    Texture2D* normal_ = nullptr;
    Texture2D* specular_ = nullptr;
    Texture2D* height_ = nullptr;
    Vec4 base_color_;
};

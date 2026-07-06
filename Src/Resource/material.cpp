#include "material.h"

#include "texture_manager.h"
#include "vertex_types.h"
#include "../Engine/render_system.h"
#include "../Game/GameMain.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/shader.h"
#include "../Resource/texture2D.h"
#include "../Graphics/descriptor_heap.h"

Material::Material()
{
    if (!Create(game_main->GetRenderSystem()->GetDevice(), L"Shaders/triangle.vs.hlsl", L"Shaders/triangle.ps.hlsl",
                kStaticVertexLayout))
    {
        MessageBox(nullptr, L"Failed to create material", L"Error", MB_OK);
    }
    
}

Material::Material(const MeshMaterialDesc& desc)
{
    if (!Create(game_main->GetRenderSystem()->GetDevice(), L"Shaders/triangle.vs.hlsl", L"Shaders/triangle.ps.hlsl",
                kStaticVertexLayout))
    {
        MessageBox(nullptr, L"Failed to create material", L"Error", MB_OK);
    }
    if (!desc.diffuse_texture_path.empty())
    {
        Texture2D* texture = TextureManager::Get().Load(desc.diffuse_texture_path.c_str());
        if (texture)
        {
            SetDiffuse(texture);
        }
        else
        {
            printf("Failed to load texture %ls\n", desc.diffuse_texture_path.c_str());
        }
    }
    if (!desc.normal_texture_path.empty())
    {
        Texture2D* normal = TextureManager::Get().Load(desc.normal_texture_path.c_str(), false);
        if (normal)
        {
            SetNormal(normal);
        }
        else
        {
            printf("Failed to load normal %ls\n", desc.normal_texture_path.c_str());
        }
    }

    if (!desc.specular_texture_path.empty())
    {
        Texture2D* specular = TextureManager::Get().Load(desc.specular_texture_path.c_str(), false);
        if (specular)
        {
            SetSpecular(specular);
        }
        else
        {
            printf("Failed to load specular %ls\n", desc.specular_texture_path.c_str());
        }
    }
    if (!desc.height_texture_path.empty())
    {
        Texture2D* height = TextureManager::Get().Load(desc.height_texture_path.c_str(), false);
        if (height)
        {
            SetHeight(height);
        }
        else
        {
            printf("Failed to load height %ls\n", desc.height_texture_path.c_str());
        }
    }
    SetBaseColor(desc.base_color);
    SetRoughness(desc.roughness);
    SetMetallic(desc.metallic);
}

bool Material::Create(ID3D12Device* device, const wchar_t* vs_path, const wchar_t* ps_path,
                      std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout)
{
    
    base_color_ = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    root_signature_ = std::make_unique<RootSignature>();
    RootSignatureBuilder builder;
    builder
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX) //b0
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL) //t0
        .AddCbv(1, D3D12_SHADER_VISIBILITY_PIXEL) // b1
        .AddCbv(2, D3D12_SHADER_VISIBILITY_PIXEL) // b2 
        .AddSrvTable(1, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t1
        .AddSrvTable(2, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t2
        .AddSrvTable(3, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP)
        .AddComparisonSampler(1, D3D12_SHADER_VISIBILITY_PIXEL); // s1

    if (!builder.Build(device, root_signature_.get()))
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
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = input_layout.data();
    layout_desc.NumElements =
        static_cast<UINT>(input_layout.size());

    PipelineStateBuilder ps_builder;
    ps_builder
        .SetRootSignature(root_signature_->GetRootSignature())
        .SetVertexShader(vertex_shader_->GetBytecode())
        .SetPixelShader(pixel_shader_->GetBytecode())
        .SetInputLayout(layout_desc);


    if (!ps_builder.Build(device, pipeline_state_.get()))
    {
        MessageBox(nullptr, L"Failed to create pipeline state", L"Error", MB_OK);
        return false;
    }
    return true;
}

void Material::Apply(ID3D12GraphicsCommandList* command_list, const DescriptorHeap* descriptor_heap) const
{
    command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    ID3D12DescriptorHeap* heaps[] = {descriptor_heap->GetHeap()};
    command_list->SetDescriptorHeaps(1, heaps);
    const uint32_t srv_index = (diffuse_ != nullptr) ? diffuse_->GetSrvIndex() : 0;
    command_list->SetGraphicsRootDescriptorTable(1, descriptor_heap->GetGpuHandle(srv_index));
    const uint32_t norm_index = (normal_ != nullptr) ? normal_->GetSrvIndex() : 0;
    command_list->SetGraphicsRootDescriptorTable(4, descriptor_heap->GetGpuHandle(norm_index));
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

void Material::SetBaseColor(const Vec4& color)
{
    base_color_ = color;
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

Vec4 Material::GetBaseColor() const
{
    return base_color_;
}

void Material::SetRoughness(float roughness)
{
    roughness_ = roughness;
}

void Material::SetMetallic(float metallic)
{
    metallic_ = metallic;
}

float Material::GetRoughness() const
{
    return roughness_;
}

float Material::GetMetallic() const
{
    return metallic_;
}

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
#include "../Renderer/cb_file.h"
#include "../Graphics/root_param.h"

Material::Material()
{

}

Material::Material(const MeshMaterialDesc& desc)
{
    if (!desc.diffuse_texture_path.empty())
    {
        Texture2D* texture = TextureManager::Get().Load(desc.diffuse_texture_path.c_str());
        if (texture)
        {
            SetDiffuse(texture);
            flag_ |= kMatHasTexture;
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
            flag_ |= kMatHasNormalMap;
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
            flag_ |= kMatHasSpecular;
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
            flag_ |= kMatHasHeight;
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

void Material::SetHeightScale(float scale)
{
    height_scale_ = scale;
}

float Material::GetHeightScale() const
{
    return height_scale_;
}

float Material::GetRoughness() const
{
    return roughness_;
}

float Material::GetMetallic() const
{
    return metallic_;
}

uint32 Material::GetHasFlag() const
{
    return flag_;
}

MaterialBinding Material::GetBinding() const
{
    MaterialBinding binding = {};
    binding.diffuse_srv = (flag_ & kMatHasTexture) ? diffuse_->GetSrvIndex() : 0;
    binding.normal_srv = (flag_ & kMatHasNormalMap) ? normal_->GetSrvIndex() : 0;
    binding.specular_srv = (flag_ & kMatHasSpecular) ? specular_->GetSrvIndex() : 0;
    binding.height_srv = (flag_ & kMatHasHeight) ? height_->GetSrvIndex() : 0;
    binding.cb.base_color = base_color_;
    binding.cb.flag = flag_;
    binding.cb.metallic = metallic_;
    binding.cb.roughness = roughness_;
    binding.cb.height_scale = height_scale_;
    return binding;
}

ShaderId Material::GetShaderId() const
{
    return shader_id_;
}

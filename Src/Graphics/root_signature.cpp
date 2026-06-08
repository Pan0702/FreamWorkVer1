#include "root_signature.h"
#include "../Core/common.h"

ID3D12RootSignature* RootSignature::GetRootSignature() const
{
    return root_signature_.Get();
}

RootSignatureBuilder& RootSignatureBuilder::AddCbv(UINT shader_register, D3D12_SHADER_VISIBILITY visibility)
{
    RootParameterSpec spec = {};
    spec.type = RootParameterType::kCbv;
    spec.shader_register = shader_register;
    spec.descriptor_count = 0;
    spec.visibility = visibility;
    parameter_specs_.push_back(spec);
    return *this;
}

RootSignatureBuilder& RootSignatureBuilder::AddSrvTable(UINT shader_register, UINT descriptor_count,
                                                        D3D12_SHADER_VISIBILITY visibility)
{
    RootParameterSpec spec = {};
    spec.type = RootParameterType::kSrvTable;
    spec.shader_register = shader_register;
    spec.descriptor_count = descriptor_count;
    spec.visibility = visibility;
    parameter_specs_.push_back(spec);
    return *this;
}

RootSignatureBuilder& RootSignatureBuilder::AddStaticSampler(UINT shader_register, D3D12_SHADER_VISIBILITY visibility,
                                                             D3D12_TEXTURE_ADDRESS_MODE address_mode)
{
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = address_mode;
    sampler.AddressV = address_mode;
    sampler.AddressW = address_mode;
    sampler.MipLODBias = 0.0f;
    sampler.MaxAnisotropy = 1;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = shader_register;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = visibility;
    static_samplers_.push_back(sampler);
    return *this;
}

bool RootSignatureBuilder::Build(ID3D12Device* device, RootSignature* out_root_signature) const
{
    if (device == nullptr || out_root_signature == nullptr)
    {
        MessageBox(nullptr, L"Invalid parameter", L"Error", MB_OK);
        return false;
    }

    std::vector<D3D12_DESCRIPTOR_RANGE> descriptor_ranges;

    for (const RootParameterSpec& spec : parameter_specs_)
    {
        if (spec.type != RootParameterType::kSrvTable)
        {
            continue;
        }

        D3D12_DESCRIPTOR_RANGE range{};
        range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        range.NumDescriptors = spec.descriptor_count;
        range.BaseShaderRegister = spec.shader_register;
        range.RegisterSpace = 0;
        range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        descriptor_ranges.push_back(range);
    }

    std::vector<D3D12_ROOT_PARAMETER> root_parameters;
    root_parameters.reserve(parameter_specs_.size());

    size_t srv_table_index = 0;

    for (const RootParameterSpec& spec : parameter_specs_)
    {
        D3D12_ROOT_PARAMETER root_parameter{};

        switch (spec.type)
        {
        case RootParameterType::kCbv:
            root_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            root_parameter.Descriptor.ShaderRegister = spec.shader_register;
            root_parameter.Descriptor.RegisterSpace = 0;
            root_parameter.ShaderVisibility = spec.visibility;
            break;

        case RootParameterType::kSrvTable:
            root_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            root_parameter.DescriptorTable.NumDescriptorRanges = 1;
            root_parameter.DescriptorTable.pDescriptorRanges = &descriptor_ranges[srv_table_index];
            root_parameter.ShaderVisibility = spec.visibility;

            ++srv_table_index;
            break;
        }

        root_parameters.push_back(root_parameter);
    }

    D3D12_ROOT_SIGNATURE_DESC desc{};
    desc.NumParameters = static_cast<UINT>(root_parameters.size());
    desc.pParameters = root_parameters.data();
    desc.NumStaticSamplers = static_cast<UINT>(static_samplers_.size());
    desc.pStaticSamplers = static_samplers_.data();
    desc.Flags = flags_;

    Microsoft::WRL::ComPtr<ID3DBlob> signature_blob;
    Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                             &signature_blob, &error_blob);

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to serialize root signature", L"Error", MB_OK);
        return false;
    }

    hr = device->CreateRootSignature(0, signature_blob->GetBufferPointer(),
                                     signature_blob->GetBufferSize(),
                                     IID_PPV_ARGS(&out_root_signature->root_signature_));

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create root signature", L"Error", MB_OK);
        return false;
    }

    return true;
}

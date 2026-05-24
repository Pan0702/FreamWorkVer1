#include "root_signature.h"
#include "../Core/common.h"

bool RootSignature::Initialize(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_RANGE range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0;
    range.OffsetInDescriptorsFromTableStart = 0;
    range.RegisterSpace = 0;

    D3D12_ROOT_PARAMETER root_parameters[2] = {};
    root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    root_parameters[0].Descriptor.ShaderRegister = 0;
    root_parameters[0].Descriptor.RegisterSpace = 0;
    root_parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    root_parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    root_parameters[1].DescriptorTable.NumDescriptorRanges = 1;
    root_parameters[1].DescriptorTable.pDescriptorRanges = &range;
    root_parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// 線形補間
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// UV>1 で繰り返し
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MipLODBias = 0.0f;// ミップバイアス
    sampler.MaxAnisotropy = 0;// 異方性なし
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// シャドウ用比較は無効
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;// s0
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PS で使う

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = 2;
    desc.pParameters = root_parameters;
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &sampler;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature_blob;
    ComPtr<ID3DBlob> error_blob;
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_blob);
    if (error_blob)
    {
        OutputDebugStringA(static_cast<const char*>(error_blob->GetBufferPointer()));
    }
    if (FAILED(hr))
    {
        return false;
    }
    hr = device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(),
                                     IID_PPV_ARGS(&root_signature_));
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

ID3D12RootSignature* RootSignature::GetRootSignature() const
{
    return root_signature_.Get();
}

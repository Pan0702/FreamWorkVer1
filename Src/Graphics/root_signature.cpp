#include "root_signature.h"
#include "../Core/common.h"
bool RootSignature::Initialize(ID3D12Device* device)
{
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = 0;
    desc.pParameters = nullptr;
    desc.NumStaticSamplers = 0;
    desc.pStaticSamplers = nullptr;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    D3D12_ROOT_PARAMETER root_parameters = {};
    root_parameters.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    root_parameters.Descriptor.ShaderRegister = 0;
    root_parameters.Descriptor.RegisterSpace = 0;
    root_parameters.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    
    desc.NumParameters = 1;
    desc.pParameters = &root_parameters;
    
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
    hr = device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature_));
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

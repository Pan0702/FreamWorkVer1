#pragma once
#include <d3d12.h>
#include <vector>

#include "../Core/common.h"

class RootSignature
{
public:
    ID3D12RootSignature* GetRootSignature() const;

private:
    friend class RootSignatureBuilder;
    ComPtr<ID3D12RootSignature> root_signature_;
};

class RootSignatureBuilder
{
public:
    RootSignatureBuilder& AddCbv(UINT shader_register, D3D12_SHADER_VISIBILITY visibility);

    RootSignatureBuilder& AddSrvTable(UINT shader_register, UINT descriptor_count,
                                      D3D12_SHADER_VISIBILITY visibility);

    RootSignatureBuilder& AddStaticSampler(UINT shader_register, D3D12_SHADER_VISIBILITY visibility,
                                           D3D12_TEXTURE_ADDRESS_MODE address_mode);

    bool Build(ID3D12Device* device,
               RootSignature* out_root_signature) const;

private:
    enum class RootParameterType {
        kCbv,
        kSrvTable,
    };

    struct RootParameterSpec {
        RootParameterType type;
        UINT shader_register = 0;
        UINT descriptor_count = 0;
        D3D12_SHADER_VISIBILITY visibility =
            D3D12_SHADER_VISIBILITY_ALL;
    };
    
    std::vector<RootParameterSpec> parameter_specs_;
    std::vector<D3D12_STATIC_SAMPLER_DESC> static_samplers_;
    D3D12_ROOT_SIGNATURE_FLAGS flags_ = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
};

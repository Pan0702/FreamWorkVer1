#pragma once
#include <d3d12.h>
#include "../Core/common.h"

class RootSignature
{
public:
    bool Initialize(ID3D12Device* device);
    ID3D12RootSignature* GetRootSignature() const;
private:
    ComPtr<ID3D12RootSignature> root_signature_;
};

#pragma once
#include <d3d12.h>
#include "../Core/common.h"
class Shader
{
public:
    bool LoadFromFile(const wchar_t* filename,const char* entry_point,const char* target);
    bool IsValid() const;
    D3D12_SHADER_BYTECODE GetBytecode() const ;
    
private:
    ComPtr<ID3DBlob> bytecode_;
};

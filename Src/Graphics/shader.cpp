#include "shader.h"

bool Shader::LoadFromFile(const wchar_t* filename, const char* entry_point, const char* target)
{
    UINT flags = 0; 
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ComPtr<ID3DBlob> error_blob;
    HRESULT hr = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, target, flags, 0, &bytecode_, &error_blob);
    if (error_blob)
    {
        OutputDebugStringA(static_cast<const char*>(error_blob->GetBufferPointer()));
    }
    
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

bool Shader::IsValid()
{
    return bytecode_ != nullptr;
}

D3D12_SHADER_BYTECODE Shader::GetBytecode() const
{
    D3D12_SHADER_BYTECODE bytecode = {};
    bytecode.pShaderBytecode = bytecode_->GetBufferPointer();
    bytecode.BytecodeLength = bytecode_->GetBufferSize();
    return bytecode;
}

#pragma once
#include "../Core/common.h"

class GraphicsDevice
{
public:
    bool Initialize(bool enable_debug_layer);
    
    ID3D12Device* GetDevice() const;
    IDXGIFactory6* GetFactory() const;
    bool CreateFactory(bool enable_debug_layer);
    bool CreateAdapter();
    bool CreateDevice();

private:
    ComPtr<IDXGIFactory6> factory_;
    ComPtr<IDXGIAdapter1> adapter_;
    ComPtr<ID3D12Device> device_;
};

#include "graphics_device.h"

bool GraphicsDevice::Initialize(bool enable_debug_layer)
{
    if (enable_debug_layer)
    {
        ComPtr<ID3D12Debug> debug_controller;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        {
            debug_controller->EnableDebugLayer();
        }
    }
    if (!CreateFactory(enable_debug_layer))
    {
        MessageBox(nullptr, L"Failed to create DXGI factory", L"Error", MB_OK);
        return false;
    }
    if (!CreateAdapter())
    {
        MessageBox(nullptr, L"Failed to create DXGI adapter", L"Error", MB_OK);
        return false;
    }
    if (!CreateDevice())
    {
        MessageBox(nullptr, L"Failed to create D3D12 device", L"Error", MB_OK);
        return false;
    }
    
    return true;
}

ID3D12Device* GraphicsDevice::GetDevice() const
{
    return device_.Get();   
}

IDXGIFactory6* GraphicsDevice::GetFactory() const
{
    return factory_.Get();  
}

bool GraphicsDevice::CreateFactory(bool enable_debug_layer)
{
    UINT factory_flag = 0;
    if (enable_debug_layer)
    {
        factory_flag |= DXGI_CREATE_FACTORY_DEBUG;
    }
    HRESULT hr = CreateDXGIFactory2(factory_flag, IID_PPV_ARGS(&factory_));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool GraphicsDevice::CreateAdapter()
{
    for (UINT index = 0; ; ++index)
    {
        ComPtr<IDXGIAdapter1> adapter;
        HRESULT hr = factory_->EnumAdapterByGpuPreference(
            index,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&adapter));
        if (hr == DXGI_ERROR_NOT_FOUND)
        {
            break;
        }
        if (FAILED(hr))
        {
            return false;
        }

        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        hr = D3D12CreateDevice(
            adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device),
            nullptr);
        if (SUCCEEDED(hr))
        {
            adapter_ = adapter;
            return true;
        }
    }
    return false;
}

bool GraphicsDevice::CreateDevice()
{
    //TODO
    HRESULT hr = D3D12CreateDevice(adapter_.Get(), D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device_));
    if (FAILED(hr))
    {
        return false;
    }
    //ID3D12Device5‚ðŽg‚¤‚±‚Æ‚É‚È‚Á‚½‚ç‚â‚éB‚ê‚¢‚Æ‚ê‚Æ‚©
    // HRESULT hr1 = device_->QueryInterface(IID_PPV_ARGS(&device_));
    // if (FAILED(hr1))
    // {
    //     return false;
    // }
    return true;   
}

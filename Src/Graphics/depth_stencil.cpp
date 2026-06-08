#include "depth_stencil.h"
#include "../Platform/window.h"

bool DepthStencil::Initialize(ID3D12Device* device, uint32_t width, uint32_t height)
{
    device_ = device;
    if (!heap_.Initialize(device,D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false))
    {
        return false;
    }
    return CreateResource(width, height);
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencil::GetCpuHandle() const
{
    return heap_.GetCpuHandle(0);
}

bool DepthStencil::Resize(uint32_t w, uint32_t h)
{
    depth_stencil_.Reset();
    return CreateResource(w, h);
}

bool DepthStencil::CreateResource(uint32_t w, uint32_t h)
{
    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;


    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment = 0;
    desc.Width = w;
    desc.Height = h;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = kFORMAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = kFORMAT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;

    HRESULT hr = device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &desc,
                                                  D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value,IID_PPV_ARGS(&depth_stencil_));
    if (FAILED(hr))
    {
        return false;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = kFORMAT;
    dsv_desc.ViewDimension =
        D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Texture2D.MipSlice = 0;

    device_->CreateDepthStencilView(depth_stencil_.Get(), &dsv_desc, heap_.GetCpuHandle(0));

    return true;
}

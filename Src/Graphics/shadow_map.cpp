#include "shadow_map.h"

bool ShadowMap::Initialize(ID3D12Device* device, uint32 size, DescriptorHeap* srv_heap)
{
    device_ = device;
    size_ = size;
    srv_heap_ = srv_heap;
    
    //　専用の小さなヒープの作成
    if (!dsv_heap_.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false))
    {
        return false;
    }
    return Create();
}

bool ShadowMap::Create()
{
    // GPUは読み書き、読み取りに使うからCPUから直接書き換えない。
    // ShadowMapはGPUが描画結果を書き込みps_shaderで読むテクスチャなので
    // Defaultを使用する。
    D3D12_HEAP_PROPERTIES heap_properties = {};
    heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_properties.CreationNodeMask = 1;
    heap_properties.VisibleNodeMask = 1;
    
    //　シャドウマップ本体になる2Dテクスチャの作成
    //　らいとから見た深度だけを保持するので正方形テクスチャにする。
    D3D12_RESOURCE_DESC resource_desc = {};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resource_desc.Alignment = 0;
    resource_desc.Width = size_;
    resource_desc.Height = size_;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    
    // 同じリソースをDSVでは深度バッファ、
    // SRVでは1chテクスチャとして見る。
    //　そのためリソース本体は型を固定しない。TYPELESSを使用
    resource_desc.Format = DXGI_FORMAT_R32_TYPELESS;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    
    // 深度バッファをクリアするときの初期値
    //　1.0fは深度の最奥だから、描画された場所だけがより手前の深度地で上書きされる。
    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    clear_value.DepthStencil.Depth = 1.0f;
    clear_value.DepthStencil.Stencil = 0;
    
    //Shadowマップ用のＤＳＶを作成
    //最初の用とはシャドウパスで深度書き込みなのでDEPTH_WRITE状態で作成する。
    HRESULT hr = device_->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc,
                                                 D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clear_value, IID_PPV_ARGS(&texture_));
    if (FAILED(hr))
    {
        return false;
    }
    
    //作成したTYPELESSリソースを深度書き込み用にDSVとしてみるための設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Texture2D.MipSlice = 0;

    device_->CreateDepthStencilView(texture_.Get(), &dsv_desc, dsv_heap_.GetCpuHandle(0));

    //psからShadowMapを読むため、共通のSRVヒープに1枠確保する
    UINT index = 0;
    if (!srv_heap_->Allocate(index))
    {
        return false;
    }
    srv_index_ = index;
    
    // R32_FLOAT として読むことで、D32_FLOAT に書いた深度を 1ch float として扱う。
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    device_->CreateShaderResourceView(texture_.Get(), &srv_desc, srv_heap_->GetCpuHandle(index));
    return true;   
}

D3D12_CPU_DESCRIPTOR_HANDLE ShadowMap::GetDRV() const
{
    return dsv_heap_.GetCpuHandle(0);
}

uint32 ShadowMap::GetSrvIndex() const
{
    return srv_index_;
}

ID3D12Resource* ShadowMap::GetResource() const
{
    return texture_.Get();
}

uint32 ShadowMap::GetSize() const
{
    return size_;
}



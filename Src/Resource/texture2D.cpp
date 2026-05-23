#include "texture2D.h"

bool Texture2D::Initialize(ID3D12Device* device, ID3D12CommandQueue* queue, ID3D12GraphicsCommandList* cmd_list,
                           ID3D12CommandAllocator* allocator, const LoadedImage& image)
{
    //テクスチャリソース作成
    D3D12_HEAP_PROPERTIES heap_props = {};
    heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_props.CreationNodeMask = 1;
    heap_props.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resource_desc = {};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resource_desc.Alignment = 0;
    resource_desc.Width = image.width;
    resource_desc.Height = image.height;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
                                                 D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&texture_));
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create CreateCommittedResource File:texture2D", L"Error", MB_OK);
        return false;
    }

    //アップロードバッファー作成

    D3D12_HEAP_PROPERTIES upload_heap_props = {};
    upload_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
    upload_heap_props.CPUPageProperty =
        D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    upload_heap_props.MemoryPoolPreference =
        D3D12_MEMORY_POOL_UNKNOWN;
    upload_heap_props.CreationNodeMask = 1;
    upload_heap_props.VisibleNodeMask = 1;

    uint32_t row_size = image.width * 4;
    uint32_t aligned_row_pitch = (row_size + 255) & ~255u;
    UINT64 upload_size = static_cast<UINT64>(aligned_row_pitch) * image.height;
    D3D12_RESOURCE_DESC upload_desc = {};
    upload_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    upload_desc.Width = upload_size;
    upload_desc.Height = 1;
    upload_desc.DepthOrArraySize = 1;
    upload_desc.MipLevels = 1;
    upload_desc.Format = DXGI_FORMAT_UNKNOWN;
    upload_desc.SampleDesc.Count = 1;
    upload_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    upload_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    hr = device->CreateCommittedResource(&upload_heap_props, D3D12_HEAP_FLAG_NONE, &upload_desc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ,
                                         nullptr,IID_PPV_ARGS(&upload_buffer_));
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create CreateCommittedResource File:texture2D", L"Error", MB_OK);
        return false;
    }

    // MapのデータをMemcpyしてUnmap
    uint8_t* mapped = nullptr;
    D3D12_RANGE read_range = {0,0};
    hr = upload_buffer_ ->Map(0, &read_range, reinterpret_cast<void**>(&mapped));
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to Map File:texture2D", L"Error", MB_OK);
        return false;
    }
    
    uint32_t src_row_pitch = image.width * 4;
    const uint8_t* src = image.pixels.data();
    for (uint32_t y = 0; y < image.height; ++y)
    {
        memcpy_s(y * aligned_row_pitch + mapped, aligned_row_pitch, src, src_row_pitch);
        src += src_row_pitch;
    }
    upload_buffer_->Unmap(0, nullptr);
    
    
    // CopyTextureRegion記録
    
    D3D12_TEXTURE_COPY_LOCATION src_loc = {};
    src_loc.pResource = upload_buffer_.Get();
    src_loc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src_loc.PlacedFootprint.Offset = 0;
    src_loc.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    src_loc.PlacedFootprint.Footprint.Width = image.width;
    src_loc.PlacedFootprint.Footprint.Height = image.height;
    src_loc.PlacedFootprint.Footprint.Depth = 1;
    src_loc.PlacedFootprint.Footprint.RowPitch = aligned_row_pitch;
    
    //　destination(コピー先)の略
    D3D12_TEXTURE_COPY_LOCATION dst_loc = {};
    dst_loc.pResource = texture_.Get();
    dst_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst_loc.SubresourceIndex = 0;
    
    cmd_list->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
    // ===== Step 5: テクスチャ状態を COPY_DEST →PIXEL_SHADER_RESOURCE =====
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = texture_.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    cmd_list->ResourceBarrier(1, &barrier);

    // コマンドリストを閉じて実行して、待機
    cmd_list->Close();
    ID3D12CommandList* command_lists[] = {cmd_list};
    queue->ExecuteCommandLists(1, command_lists);
    
    //GPU完了待ち(Fence同期）
    ComPtr<ID3D12Fence> fence;
    device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence));
    HANDLE event = CreateEventW(nullptr,FALSE,FALSE,nullptr);
    queue->Signal(fence.Get(),1);
    fence->SetEventOnCompletion(1,event);
    WaitForSingleObject(event,INFINITE);
    CloseHandle(event);
    return true;
}

void Texture2D::CreateSrv(ID3D12Device* device, D3D12_CPU_DESCRIPTOR_HANDLE srv_handle)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    device->CreateShaderResourceView(texture_.Get(), &srv_desc, srv_handle);
}

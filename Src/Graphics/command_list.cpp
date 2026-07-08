#include "command_list.h"

CommandList::CommandList() = default;
CommandList::~CommandList() = default;

bool CommandList::Initialize(ID3D12Device* device)
{
    if (!CreateCommandAllocator(device))
    {
        MessageBox(nullptr, L"Failed to create command allocator", L"Error", MB_OK);
        return false;
    }

    if (!CreateCommandList(device))
    {
        MessageBox(nullptr, L"Failed to create command list", L"Error", MB_OK);
        return false;
    }

    return true;
}

bool CommandList::Reset(ID3D12CommandAllocator* allocator) const
{
    HRESULT hr = allocator->Reset();
    if (FAILED(hr))
    {
        return false;
    }

    hr = command_list_->Reset(allocator, nullptr);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool CommandList::Close() const
{
    HRESULT hr = command_list_->Close();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

ID3D12GraphicsCommandList* CommandList::GetCommandList() const
{
    return command_list_.Get();
}

ID3D12CommandAllocator* CommandList::GetCommandAllocator() const
{
    return command_allocator_.Get();
}

bool CommandList::CreateCommandAllocator(ID3D12Device* device)
{
    HRESULT hr = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&command_allocator_));
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool CommandList::CreateCommandList(ID3D12Device* device)
{
    HRESULT hr = device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        command_allocator_.Get(),
        nullptr,
        IID_PPV_ARGS(&command_list_));
    if (FAILED(hr))
    {
        return false;
    }

    hr = command_list_->Close();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

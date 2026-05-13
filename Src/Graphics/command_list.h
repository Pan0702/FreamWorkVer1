#pragma once
#include "../Core/common.h"

class CommandList
{
public:
    CommandList();
    ~CommandList();

    bool Initialize(ID3D12Device* device);
    bool Reset();
    bool Close();

    ID3D12GraphicsCommandList* GetCommandList() const;
    ID3D12CommandAllocator* GetCommandAllocator() const;

private:
    bool CreateCommandAllocator(ID3D12Device* device);
    bool CreateCommandList(ID3D12Device* device);

    ComPtr<ID3D12CommandAllocator> command_allocator_;
    ComPtr<ID3D12GraphicsCommandList> command_list_;
};

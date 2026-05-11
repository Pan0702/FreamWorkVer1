#pragma once
#include "../Core/common.h"
class CommandQueue
{
public:
    CommandQueue();
    ~CommandQueue();
    bool Initialize(ID3D12Device* device);
    uint64_t Signal();
    void WaitForFence(uint64_t fence_value);
    void WaitIdle();
    
    ID3D12CommandQueue* GetCommandQueue() const;
    
private:
    bool CreateCommandQueue(ID3D12Device* device);
    bool CreateFence(ID3D12Device* device);
    ComPtr<ID3D12CommandQueue> command_queue_;
    ComPtr<ID3D12Fence> fence_;
    uint64_t fence_value_ = 0;
    HANDLE fence_event_ = nullptr;
    
};

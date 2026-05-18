#include "command_queue.h"

#include <assert.h>

CommandQueue::CommandQueue()
{
}

CommandQueue::~CommandQueue()
{
    if (fence_event_ != nullptr) {
        CloseHandle(fence_event_);
    }
}

bool CommandQueue::Initialize(ID3D12Device* device)
{
    if (!CreateCommandQueue(device))
    {
        MessageBox(NULL, L"Failed to create command queue", L"Error", MB_OK);
        return false;
    }
    if (!CreateFence(device))
    {
        MessageBox(NULL, L"Failed to create fence", L"Error", MB_OK);
        return false;
    }
    return true;
}

bool CommandQueue::CreateCommandQueue(ID3D12Device* device)
{
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;   //�O���t�B�b�N�X�R�}���h�p
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;   //�ʏ�̓t���O�Ȃ�
    queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;  //�ʏ�D��
    HRESULT hr = device->CreateCommandQueue
                        (&queue_desc, IID_PPV_ARGS(&command_queue_));
    if (FAILED(hr))
    {
        return false;
    }
    return true;  
}


bool CommandQueue::CreateFence(ID3D12Device* device)
{
    HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));

    if (FAILED(hr))
    {
        return false;
    }
    fence_event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fence_event_ == nullptr) {
        return false;
    }
    return true;
}

uint64_t CommandQueue::Signal()
{
    ++fence_value_;
    HRESULT hr = command_queue_->Signal(fence_.Get(), fence_value_);
    if (FAILED(hr))
    {
        return 0;
    }
    return fence_value_;   
}

void CommandQueue::WaitForFence(uint64_t fence_value) const
{
    if (fence_->GetCompletedValue() >= fence_value)
    {
        return;
    }
    HRESULT hr = fence_->SetEventOnCompletion(fence_value, fence_event_);
    if (FAILED(hr))
    {
        assert(false);
        return;
    }
    WaitForSingleObject(fence_event_, INFINITE);
}

void CommandQueue::WaitIdle()
{
    const uint64_t fence_value = Signal();
    WaitForFence(fence_value);  
}

ID3D12CommandQueue* CommandQueue::GetCommandQueue() const
{
    return command_queue_.Get(); 
}




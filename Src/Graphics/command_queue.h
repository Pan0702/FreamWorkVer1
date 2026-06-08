#pragma once
#include "../Core/common.h"

/**
 * @brief CommandQueueのデータと処理をまとめる型。
 */
class CommandQueue
{
public:
    /**
     * @brief CommandQueueを初期化するコンストラクタ。
     */
    CommandQueue();
    /**
     * @brief CommandQueueの終了処理を行うデストラクタ。
     */
    ~CommandQueue();

    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief Signalを行う関数。
     * @return 戻り値。
     */
    uint64_t Signal();
    /**
     * @brief WaitForFenceを行う関数。
     * @param fence_value 引数。
     */
    void WaitForFence(uint64_t fence_value) const;
    /**
     * @brief WaitIdleを行う関数。
     */
    void WaitIdle();

    /**
     * @brief CommandQueueを取得する関数。
     * @return 戻り値。
     */
    ID3D12CommandQueue* GetCommandQueue() const;

private:
    /**
     * @brief CreateCommandQueueを行う関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool CreateCommandQueue(ID3D12Device* device);
    /**
     * @brief CreateFenceを行う関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool CreateFence(ID3D12Device* device);

    ComPtr<ID3D12CommandQueue> command_queue_;
    ComPtr<ID3D12Fence> fence_;
    uint64_t fence_value_ = 0;
    HANDLE fence_event_ = nullptr;
};

#pragma once
#include "../Core/common.h"

/**
 * @brief CommandQueueのデータと処理をまとめる型。
 */
class CommandQueue
{
public:
    /**
     * @brief 値を初期化する。
     */
    CommandQueue();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~CommandQueue();

    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief GPU や OS 側へ処理を渡し、必要な同期を進める。
     * @return フェンスへ新しい値を通知できた場合は true。
     */
    uint64_t Signal();
    /**
     * @brief GPU や OS 側へ処理を渡し、必要な同期を進める。
     * @param fence_value 待機するフェンス値。
     */
    void WaitForFence(uint64_t fence_value) const;
    /**
     * @brief GPU や OS 側へ処理を渡し、必要な同期を進める。
     */
    void WaitIdle();

    /**
     * @brief D3D12 コマンドキュー を取得する。
     * @return 保持している D3D12 コマンドキュー。未作成なら nullptr。
     */
    ID3D12CommandQueue* GetCommandQueue() const;

private:
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateCommandQueue(ID3D12Device* device);
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateFence(ID3D12Device* device);

    ComPtr<ID3D12CommandQueue> command_queue_;
    ComPtr<ID3D12Fence> fence_;
    uint64_t fence_value_ = 0;
    HANDLE fence_event_ = nullptr;
};

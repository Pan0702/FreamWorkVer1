#pragma once
#include "../Core/common.h"

/**
 * @brief CommandListのデータと処理をまとめる型。
 */
class CommandList
{
public:
    /**
     * @brief 値を初期化する。
     */
    CommandList();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~CommandList();

    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 再利用できるように内部状態を初期位置へ戻す。
     * @return 内部状態を再利用できる状態へ戻せた場合は true。
     */
    bool Reset();
    /**
     * @brief 記録中のコマンドリストを閉じて実行可能にする。
     * @return コマンドリストを実行可能な状態で閉じられた場合は true。
     */
    bool Close();

    /**
     * @brief D3D12 コマンドリスト を取得する。
     * @return 保持している D3D12 コマンドリスト。未作成なら nullptr。
     */
    ID3D12GraphicsCommandList* GetCommandList() const;
    /**
     * @brief D3D12 コマンドアロケータ を取得する。
     * @return 保持している D3D12 コマンドアロケータ。未作成なら nullptr。
     */
    ID3D12CommandAllocator* GetCommandAllocator() const;

private:
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateCommandAllocator(ID3D12Device* device);
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateCommandList(ID3D12Device* device);

    ComPtr<ID3D12CommandAllocator> command_allocator_;
    ComPtr<ID3D12GraphicsCommandList> command_list_;
};

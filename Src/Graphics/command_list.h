#pragma once
#include "../Core/common.h"

/**
 * @brief CommandListのデータと処理をまとめる型。
 */
class CommandList
{
public:
    /**
     * @brief CommandListを初期化するコンストラクタ。
     */
    CommandList();
    /**
     * @brief CommandListの終了処理を行うデストラクタ。
     */
    ~CommandList();

    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 再利用できるように状態をリセットする関数。
     * @return 条件を満たす場合は true。
     */
    bool Reset();
    /**
     * @brief Closeを行う関数。
     * @return 条件を満たす場合は true。
     */
    bool Close();

    /**
     * @brief CommandListを取得する関数。
     * @return 戻り値。
     */
    ID3D12GraphicsCommandList* GetCommandList() const;
    /**
     * @brief CommandAllocatorを取得する関数。
     * @return 戻り値。
     */
    ID3D12CommandAllocator* GetCommandAllocator() const;

private:
    /**
     * @brief CreateCommandAllocatorを行う関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool CreateCommandAllocator(ID3D12Device* device);
    /**
     * @brief CreateCommandListを行う関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool CreateCommandList(ID3D12Device* device);

    ComPtr<ID3D12CommandAllocator> command_allocator_;
    ComPtr<ID3D12GraphicsCommandList> command_list_;
};

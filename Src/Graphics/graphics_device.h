#pragma once
#include "../Core/common.h"

/**
 * @brief GraphicsDeviceのデータと処理をまとめる型。
 */
class GraphicsDevice
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param enable_debug_layer D3D12 デバッグレイヤーを有効にするかどうか。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(bool enable_debug_layer);

    /**
     * @brief D3D12 デバイス を取得する。
     * @return 保持している D3D12 デバイス。未初期化なら nullptr。
     */
    ID3D12Device* GetDevice() const;
    /**
     * @brief DXGI ファクトリ を取得する。
     * @return 保持している DXGI ファクトリ。未初期化なら nullptr。
     */
    IDXGIFactory6* GetFactory() const;
    /**
     * @brief DXGI ファクトリを作成する。
     * @param enable_debug_layer D3D12 デバッグレイヤーを有効にするかどうか。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateFactory(bool enable_debug_layer);
    /**
     * @brief 描画に使用する GPU アダプタを選択する。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateAdapter();
    /**
     * @brief D3D12 デバイスを作成する。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool CreateDevice();

private:
    ComPtr<IDXGIFactory6> factory_;
    ComPtr<IDXGIAdapter1> adapter_;
    ComPtr<ID3D12Device> device_;
};

#pragma once
#include "../Core/common.h"

/**
 * @brief GraphicsDeviceのデータと処理をまとめる型。
 */
class GraphicsDevice
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param enable_debug_layer 引数。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(bool enable_debug_layer);

    /**
     * @brief Deviceを取得する関数。
     * @return 戻り値。
     */
    ID3D12Device* GetDevice() const;
    /**
     * @brief Factoryを取得する関数。
     * @return 戻り値。
     */
    IDXGIFactory6* GetFactory() const;
    /**
     * @brief CreateFactoryを行う関数。
     * @param enable_debug_layer 引数。
     * @return 条件を満たす場合は true。
     */
    bool CreateFactory(bool enable_debug_layer);
    /**
     * @brief CreateAdapterを行う関数。
     * @return 条件を満たす場合は true。
     */
    bool CreateAdapter();
    /**
     * @brief CreateDeviceを行う関数。
     * @return 条件を満たす場合は true。
     */
    bool CreateDevice();

private:
    ComPtr<IDXGIFactory6> factory_;
    ComPtr<IDXGIAdapter1> adapter_;
    ComPtr<ID3D12Device> device_;
};

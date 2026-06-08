#pragma once
#include <d3d12.h>
#include <vector>

#include "../Core/common.h"

/**
 * @brief RootSignatureのデータと処理をまとめる型。
 */
class RootSignature
{
public:
    /**
     * @brief RootSignatureを取得する関数。
     * @return 戻り値。
     */
    ID3D12RootSignature* GetRootSignature() const;

private:
    friend class RootSignatureBuilder;
    ComPtr<ID3D12RootSignature> root_signature_;
};

/**
 * @brief RootSignatureBuilderのデータと処理をまとめる型。
 */
class RootSignatureBuilder
{
public:
    /**
     * @brief 定数バッファビュー用のルートパラメータを追加する関数。
     * @param shader_register 引数。
     * @param visibility 引数。
     * @return 戻り値。
     */
    RootSignatureBuilder& AddCbv(UINT shader_register, D3D12_SHADER_VISIBILITY visibility);

    /**
     * @brief シェーダーリソースビュー用のディスクリプタテーブルを追加する関数。
     * @param shader_register 引数。
     * @param descriptor_count 引数。
     * @param visibility 引数。
     * @return 戻り値。
     */
    RootSignatureBuilder& AddSrvTable(UINT shader_register, UINT descriptor_count,
                                      D3D12_SHADER_VISIBILITY visibility);

    /**
     * @brief AddStaticSamplerを行う関数。
     * @param shader_register 引数。
     * @param visibility 引数。
     * @param address_mode 引数。
     * @return 戻り値。
     */
    RootSignatureBuilder& AddStaticSampler(UINT shader_register, D3D12_SHADER_VISIBILITY visibility,
                                           D3D12_TEXTURE_ADDRESS_MODE address_mode);

    /**
     * @brief Buildを行う関数。
     * @param device DirectX 12 デバイス。
     * @param out_root_signature 引数。
     * @return 条件を満たす場合は true。
     */
    bool Build(ID3D12Device* device,
               RootSignature* out_root_signature) const;

private:
/**
 * @brief RootParameterTypeのデータと処理をまとめる型。
 */
    enum class RootParameterType {
        kCbv,
        kSrvTable,
    };

/**
 * @brief RootParameterSpecのデータと処理をまとめる型。
 */
    struct RootParameterSpec {
        RootParameterType type;
        UINT shader_register = 0;
        UINT descriptor_count = 0;
        D3D12_SHADER_VISIBILITY visibility =
            D3D12_SHADER_VISIBILITY_ALL;
    };

    std::vector<RootParameterSpec> parameter_specs_;
    std::vector<D3D12_STATIC_SAMPLER_DESC> static_samplers_;
    D3D12_ROOT_SIGNATURE_FLAGS flags_ = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
};

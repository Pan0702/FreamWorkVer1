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
     * @brief ルートシグネチャ を取得する。
     * @return 保持しているルートシグネチャ。未作成なら nullptr。
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
     * @brief RootSignature に CBV ルートパラメータを追加する。
     * @param shader_register shader_register に設定する値。
     * @param visibility visibility に設定する値。
     * @return 保持している Cbv への参照。
     */
    RootSignatureBuilder& AddCbv(UINT shader_register, D3D12_SHADER_VISIBILITY visibility);

    /**
     * @brief RootSignature に SRV テーブルを追加する。
     * @param shader_register shader_register に設定する値。
     * @param descriptor_count descriptor_count に設定する値。
     * @param visibility visibility に設定する値。
     * @return 保持している Srv Table への参照。
     */
    RootSignatureBuilder& AddSrvTable(UINT shader_register, UINT descriptor_count,
                                      D3D12_SHADER_VISIBILITY visibility);

    /**
     * @brief RootSignature に静的サンプラを追加する。
     * @param shader_register shader_register に設定する値。
     * @param visibility visibility に設定する値。
     * @param address_mode address_mode に設定する値。
     * @return 保持している Static Sampler への参照。
     */
    RootSignatureBuilder& AddStaticSampler(UINT shader_register, D3D12_SHADER_VISIBILITY visibility,
                                           D3D12_TEXTURE_ADDRESS_MODE address_mode);

    /**
     * @brief 後続処理で使いやすい形にデータを組み立てる。
     * @param device 使用する D3D12 デバイス。
     * @param out_root_signature out_root_signature に設定する値。
     * @return 後続処理で使いやすい形にデータを組み立てる 場合は true。
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

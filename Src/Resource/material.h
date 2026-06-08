#pragma once
#include <cstdint>
#include <memory>
#include <span>

#include "mesh.h"
#include "../Core/Math/vec4.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include  "../Graphics/shader.h"
class DescriptorHeap;
class Texture2D;
/**
 * @brief Materialのデータと処理をまとめる型。
 */
class Material
{
public:
    /**
     * @brief Materialを初期化するコンストラクタ。
     */
    Material();
    /**
     * @brief Materialを初期化するコンストラクタ。
     * @param desc 引数。
     */
    Material(const MeshMaterialDesc& desc);
    /**
     * @brief DirectX 12 リソースを作成する関数。
     * @param device DirectX 12 デバイス。
     * @param vs_path 引数。
     * @param ps_path 引数。
     * @param input_layout 頂点入力レイアウト。
     * @return 条件を満たす場合は true。
     */
    bool Create(ID3D12Device* device, const wchar_t* vs_path, const wchar_t* ps_path,
                std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout);
    /**
     * @brief Applyを行う関数。
     * @param command_list DirectX 12 コマンドリスト。
     * @param descriptor_heap 引数。
     */
    void Apply(ID3D12GraphicsCommandList* command_list, DescriptorHeap* descriptor_heap);
    /**
     * @brief Diffuseを設定する関数。
     * @param diffuse 引数。
     */
    void SetDiffuse(Texture2D* diffuse);
    /**
     * @brief Normalを設定する関数。
     * @param normal 引数。
     */
    void SetNormal(Texture2D* normal);
    /**
     * @brief Specularを設定する関数。
     * @param specular 引数。
     */
    void SetSpecular(Texture2D* specular);
    /**
     * @brief Heightを設定する関数。
     * @param height 高さ。
     */
    void SetHeight(Texture2D* height);
    /**
     * @brief BaseColorを設定する関数。
     * @param color 色。
     */
    void SetBaseColor(const Vec4& color);
    /**
     * @brief Diffuseを取得する関数。
     * @return 戻り値。
     */
    Texture2D* GetDiffuse() const;
    /**
     * @brief Normalを取得する関数。
     * @return 戻り値。
     */
    Texture2D* GetNormal() const;
    /**
     * @brief Specularを取得する関数。
     * @return 戻り値。
     */
    Texture2D* GetSpecular() const;
    /**
     * @brief Heightを取得する関数。
     * @return 戻り値。
     */
    Texture2D* GetHeight() const;
    /**
     * @brief SpecularPowerを取得する関数。
     * @return 戻り値。
     */
    float GetSpecularPower() const;
    /**
     * @brief SpecularIntensityを取得する関数。
     * @return 戻り値。
     */
    float GetSpecularIntensity() const;
    /**
     * @brief SpecularPowerを設定する関数。
     * @param power 引数。
     */
    void SetSpecularPower(float power);
    /**
     * @brief SpecularIntensityを設定する関数。
     * @param intensity 引数。
     */
    void SetSpecularIntensity(float intensity);
    /**
     * @brief BaseColorを取得する関数。
     * @return 戻り値。
     */
    Vec4 GetBaseColor() const;

private:
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader> pixel_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    Texture2D* diffuse_ = nullptr;
    Texture2D* normal_ = nullptr;
    Texture2D* specular_ = nullptr;
    Texture2D* height_ = nullptr;
    float specular_power_ = 0.0f;
    float specular_intensity_ = 0.0f;   
    Vec4 base_color_;
};

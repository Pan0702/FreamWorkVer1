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
     * @brief 値を初期化する。
     */
    Material();
    /**
     * @brief 値を初期化する。
     * @param desc マテリアル作成に使う設定。
     */
    Material(const MeshMaterialDesc& desc);
    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @param vs_path 頂点シェーダーファイルのパス。
     * @param ps_path ピクセルシェーダーファイルのパス。
     * @param input_layout 頂点入力レイアウト。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool Create(ID3D12Device* device, const wchar_t* vs_path, const wchar_t* ps_path,
                std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout);
    /**
     * @brief マテリアルに必要な GPU 状態とテクスチャを設定する。
     * @param command_list 描画コマンドを書き込むコマンドリスト。
     * @param descriptor_heap SRV/CBV/UAV などを保持するディスクリプタヒープ。
     */
    void Apply(ID3D12GraphicsCommandList* command_list, DescriptorHeap* descriptor_heap);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param diffuse ディフューズテクスチャ。
     */
    void SetDiffuse(Texture2D* diffuse);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param normal 法線テクスチャ。
     */
    void SetNormal(Texture2D* normal);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param specular スペキュラテクスチャ。
     */
    void SetSpecular(Texture2D* specular);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param height 作成または変更後の高さ。
     */
    void SetHeight(Texture2D* height);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param color 設定する色。
     */
    void SetBaseColor(const Vec4& color);
    /**
     * @brief Diffuse を取得する。
     * @return Diffuse。見つからない、または未作成の場合は nullptr。
     */
    Texture2D* GetDiffuse() const;
    /**
     * @brief Normal を取得する。
     * @return Normal。見つからない、または未作成の場合は nullptr。
     */
    Texture2D* GetNormal() const;
    /**
     * @brief Specular を取得する。
     * @return Specular。見つからない、または未作成の場合は nullptr。
     */
    Texture2D* GetSpecular() const;
    /**
     * @brief 高さ を取得する。
     * @return 高さ。見つからない、または未作成の場合は nullptr。
     */
    Texture2D* GetHeight() const;
    /**
     * @brief スペキュラの強さ指数を取得する。
     * @return ハイライトの鋭さに使うスペキュラ指数。
     */
    float GetSpecularPower() const;
    /**
     * @brief スペキュラの強度を取得する。
     * @return ハイライトの明るさに使うスペキュラ強度。
     */
    float GetSpecularIntensity() const;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param power 掛け合わせる回数。
     */
    void SetSpecularPower(float power);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param intensity 反射や発光に使う強さ。
     */
    void SetSpecularIntensity(float intensity);
    /**
     * @brief ベースカラーを取得する。
     * @return マテリアルのベースカラー。
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

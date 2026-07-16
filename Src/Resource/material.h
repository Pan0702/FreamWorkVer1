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
     * @param ps_path ピクセルシェーダーファイルのパス。
     * @return 対象リソースの作成が完了した場合は true。
     */
    bool Create(ID3D12Device* device, const wchar_t* ps_path);
    /**
     * @brief マテリアルに必要な GPU 状態とテクスチャを設定する。
     * @param command_list 描画コマンドを書き込むコマンドリスト。
     * @param descriptor_heap SRV/CBV/UAV などを保持するディスクリプタヒープ。
     */
    void Apply(ID3D12GraphicsCommandList* command_list, const DescriptorHeap* descriptor_heap) const;
    void ApplyInstanced(ID3D12GraphicsCommandList* command, DescriptorHeap* heap) const;
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
     * @brief ベースカラーを取得する。
     * @return マテリアルのベースカラー。
     */
    Vec4 GetBaseColor() const;
    /**
     * @brief ラフネスを設定する。
     * @param roughness 設定するラフネス。
     */
    void SetRoughness(float roughness);
    /**
     * @brief メタリックを設定する。
     * @param metallic 設定するメタリック。
     */
    void SetMetallic(float metallic);
    /**
     * @brief 高さスケールを設定する。
     * @param scale 設定する高さスケール。
     */
    void SetHeightScale(float scale);
    /**
     * @brief 高さスケールを取得する。
     * @return マテリアルの高さスケール。
     */
    float GetHeightScale() const;
    /**
     * @brief ラフネスを取得する。
     * @return マテリアルのラフネス。
     */
    float GetRoughness() const;
    /**
     * @brief メタリックを取得する。
     * @return マテリアルのメタリック。
     */
    float GetMetallic() const;
    /**
     * @brief テクスチャの有無を示すフラグを取得する。
     * @return テクスチャの有無を示すフラグ。
     */
    uint32 GetHasFlag() const;
private:
    bool CreateMeshPipelineState(ID3D12Device* device, const wchar_t* path, std::span<const D3D12_INPUT_ELEMENT_DESC> input_layout);
    bool CreateInstancedMeshPipelineState(ID3D12Device* device, const wchar_t* path, std::span<const D3D12_INPUT_ELEMENT_DESC> input_element);
    void BindResources(ID3D12GraphicsCommandList* command_list, const DescriptorHeap* descriptor_heap) const;
    
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader> pixel_shader_;
    std::unique_ptr<Shader> instanced_vertex_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    std::unique_ptr<PipelineState> instanced_pipeline_state_;
    Texture2D* diffuse_ = nullptr;
    Texture2D* normal_ = nullptr;
    Texture2D* specular_ = nullptr;
    Texture2D* height_ = nullptr;
    Vec4 base_color_;
    uint32 flag_ = 0;
    float roughness_ = 0.8f;
    float metallic_ = 0.0f;   
    float height_scale_ = 1.0f;
};

#pragma once

#include "mesh.h"
#include "../Core/Math/vec4.h"
#include "../Graphics/pipeline_state_cache.h"
#include "../Renderer/cb_file.h"
class DescriptorHeap;
class Texture2D;

/**
 * @brief マテリアルが GPU へ渡すデータをまとめた値。
 *
 * ルートパラメータのインデックスは含めない。どのスロットへ置くかは
 * レイアウトを知っているレンダラー側が決める。
 */
struct MaterialBinding
{
    uint32 diffuse_srv = 0;
    uint32 normal_srv = 0;
    uint32 specular_srv = 0;
    uint32 height_srv = 0;
    CB::MaterialCB cb = {};
};

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

    /**
     * @brief 描画に必要なテクスチャ SRV と定数をまとめて取得する。
     * @return マテリアルのバインド情報。未設定のテクスチャは SRV 0 を返す。
     */
    MaterialBinding GetBinding() const;
    ShaderId GetShaderId() const;

private:
    Texture2D* diffuse_ = nullptr;
    Texture2D* normal_ = nullptr;
    Texture2D* specular_ = nullptr;
    Texture2D* height_ = nullptr;
    Vec4 base_color_;
    uint32 flag_ = 0;
    ShaderId shader_id_ = ShaderId::kPbr;
    float roughness_ = 0.8f;
    float metallic_ = 0.0f;
    float height_scale_ = 1.0f;
};

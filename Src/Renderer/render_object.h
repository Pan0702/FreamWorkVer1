#pragma once
#include "../Core/Math/my_math.h"
class Material;
class Mesh;
/**
 * @brief RenderObjectのデータと処理をまとめる型。
 */
class RenderObject
{
public:
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param mesh 読み込み、描画、または判定に使用するメッシュ。
     */
    void SetMesh(Mesh* mesh);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param material 描画に使用するマテリアル。
     */
    void SetMaterial(Material* material);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param transform transform に設定する値。
     */
    void SetTransform(const Mat& transform);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param priority priority に設定する値。
     */
    void SetPriority(int priority);
    /**
     * @brief 描画優先度を取得する。
     * @return 小さいほど先に扱う描画優先度。
     */
    int GetPriority() const;
    /**
     * @brief Actor が持つ Transform を取得する。
     * @return 現在保持している Transform。
     */
    Mat GetTransform() const;
    /**
     * @brief メッシュ を取得する。
     * @return メッシュ。見つからない、または未作成の場合は nullptr。
     */
    Mesh* GetMesh() const;
    /**
     * @brief マテリアル を取得する。
     * @return マテリアル。見つからない、または未作成の場合は nullptr。
     */
    Material* GetMaterial() const;
    
private:
    Mat transform_ = Identity();
    Mesh* mesh_ = nullptr;
    Material* material_ = nullptr;
    int priority_ = 0;
    
};

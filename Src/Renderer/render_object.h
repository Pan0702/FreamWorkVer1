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
     * @brief Meshを設定する関数。
     * @param mesh 引数。
     */
    void SetMesh(Mesh* mesh);
    /**
     * @brief Materialを設定する関数。
     * @param material マテリアル。
     */
    void SetMaterial(Material* material);
    /**
     * @brief Transformを設定する関数。
     * @param transform 引数。
     */
    void SetTransform(const Mat& transform);
    /**
     * @brief Priorityを設定する関数。
     * @param priority 引数。
     */
    void SetPriority(int priority);
    /**
     * @brief Priorityを取得する関数。
     * @return 戻り値。
     */
    int GetPriority() const;
    /**
     * @brief Transformを取得する関数。
     * @return 戻り値。
     */
    Mat GetTransform() const;
    /**
     * @brief Meshを取得する関数。
     * @return 戻り値。
     */
    Mesh* GetMesh() const;
    /**
     * @brief Materialを取得する関数。
     * @return 戻り値。
     */
    Material* GetMaterial() const;
    
private:
    Mat transform_ = Identity();
    Mesh* mesh_ = nullptr;
    Material* material_ = nullptr;
    int priority_ = 0;
    
};

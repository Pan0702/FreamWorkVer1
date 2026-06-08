#pragma once
#include "../Core/Math/my_math.h"

/**
 * @brief Cameraのデータと処理をまとめる型。
 */
class Camera
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param fov 引数。
     * @param aspect 引数。
     * @param near_z 引数。
     * @param far_z 引数。
     */
    void Initialize(float fov, float aspect, float near_z, float far_z);
    /**
     * @brief ViewMatrixを取得する関数。
     * @return 戻り値。
     */
    Mat GetViewMatrix();
    /**
     * @brief ProjectionMatrixを取得する関数。
     * @return 戻り値。
     */
    Mat GetProjectionMatrix();
    /**
     * @brief Forwardを取得する関数。
     * @return 戻り値。
     */
    Vec3 GetForward();
    /**
     * @brief Rightを取得する関数。
     * @return 戻り値。
     */
    Vec3 GetRight();
    /**
     * @brief Upを取得する関数。
     * @return 戻り値。
     */
    Vec3 GetUp() const;
    /**
     * @brief Aspectを設定する関数。
     * @param aspect 引数。
     */
    void SetAspect(float aspect);

    Vec3 pos_;
    Vec3 look_;
    Vec3 up_;
    Mat view_;
    Mat proj_;
private:
    float fov_;
    float aspect_;
    float near_z_;
    float far_z_;
};

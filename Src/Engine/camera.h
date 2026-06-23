#pragma once
#include "../Core/Math/my_math.h"

/**
 * @brief Cameraのデータと処理をまとめる型。
 */
class Camera
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param fov 縦方向の視野角。
     * @param aspect 画面のアスペクト比。
     * @param near_z ニアクリップ面の距離。
     * @param far_z ファークリップ面の距離。
     */
    void Initialize(float fov, float aspect, float near_z, float far_z);
    /**
     * @brief ビュー行列 を取得する。
     * @return 現在のカメラ姿勢から作成したビュー行列。
     */
    Mat GetViewMatrix();
    /**
     * @brief プロジェクション行列 を取得する。
     * @return 現在のカメラ設定から作成したプロジェクション行列。
     */
    Mat GetProjectionMatrix();
    /**
     * @brief 前方向ベクトル を取得する。
     * @return 現在の回転から求めた前方向ベクトル。
     */
    Vec3 GetForward();
    /**
     * @brief 右方向ベクトル を取得する。
     * @return 現在の回転から求めた右方向ベクトル。
     */
    Vec3 GetRight();
    /**
     * @brief 上方向ベクトル を取得する。
     * @return 現在の回転から求めた上方向ベクトル。
     */
    Vec3 GetUp() const;
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param aspect 画面のアスペクト比。
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

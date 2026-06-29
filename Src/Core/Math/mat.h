#pragma once

#include <DirectXMath.h>

#include "vec3.h"
using namespace DirectX;

/**
 * @brief Matのデータと処理をまとめる型。
 */
struct Mat : public XMFLOAT4X4
{
    /**
     * @brief 値を初期化する。
     */
    Mat() = default;

    /**
     * @brief 値を初期化する。
     * @param other 比較または参照する相手。
     */
    Mat(const XMMATRIX& other)
    {
        XMStoreFloat4x4(this, other);
    }

    /**
     * @brief 値を初期化する。
     * @param in_11 行列の 1 行 1 列の値。
     * @param in_12 行列の 1 行 2 列の値。
     * @param in_13 行列の 1 行 3 列の値。
     * @param in_14 行列の 1 行 4 列の値。
     * @param in_21 行列の 2 行 1 列の値。
     * @param in_22 行列の 2 行 2 列の値。
     * @param in_23 行列の 2 行 3 列の値。
     * @param in_24 行列の 2 行 4 列の値。
     * @param in_31 行列の 3 行 1 列の値。
     * @param in_32 行列の 3 行 2 列の値。
     * @param in_33 行列の 3 行 3 列の値。
     * @param in_34 行列の 3 行 4 列の値。
     * @param in_41 行列の 4 行 1 列の値。
     * @param in_42 行列の 4 行 2 列の値。
     * @param in_43 行列の 4 行 3 列の値。
     * @param in_44 行列の 4 行 4 列の値。
     */
    Mat(float in_11, float in_12, float in_13, float in_14,
        float in_21, float in_22, float in_23, float in_24,
        float in_31, float in_32, float in_33, float in_34,
        float in_41, float in_42, float in_43, float in_44)
    {
        _11 = in_11; _12 = in_12; _13 = in_13; _14 = in_14;
        _21 = in_21; _22 = in_22; _23 = in_23; _24 = in_24;
        _31 = in_31; _32 = in_32; _33 = in_33; _34 = in_34;
        _41 = in_41; _42 = in_42; _43 = in_43; _44 = in_44;
    }

    /**
     * @brief 値を初期化する。
     */
    operator XMMATRIX() const
    {
        return XMLoadFloat4x4((this));
    }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param rhs 計算に使用する行列または値。
     * @return 演算結果を反映した自分自身。
     */
    Mat& operator+=(const Mat& rhs)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += rhs.m[i][j];
        return *this;
    }

    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param rhs 計算に使用する行列または値。
     * @return 演算結果を反映した自分自身。
     */
    Mat& operator-=(const Mat& rhs)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= rhs.m[i][j];
        return *this;
    }

    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param rhs 計算に使用する行列または値。
     * @return 演算結果を反映した自分自身。
     */
    Mat& operator*=(const Mat& rhs)
    {
        *this = static_cast<Mat>(static_cast<XMMATRIX>(*this) * (rhs));
        return *this;
    }

    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param s スケール値または分解結果のスケール。
     * @return 演算結果を反映した自分自身。
     */
    Mat& operator*=(float s)
    {
        for (auto& i : m)
            for (float& j : i)
                j *= s;
        return *this;
    }

    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param s スケール値または分解結果のスケール。
     * @return 演算結果を反映した自分自身。
     */
    Mat& operator/=(float s)
    {
        const float inv = 1.0f / s;
        for (auto& i : m)
            for (float& j : i)
                j *= inv;
        return *this;
    }
};

/**
 * @brief 演算子 operator+ で値を扱う。
 * @param lhs 計算に使用する行列または値。
 * @param rhs 計算に使用する行列または値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator+(Mat lhs, const Mat& rhs) { return lhs += rhs; }
/**
 * @brief 演算子 operator- で値を扱う。
 * @param lhs 計算に使用する行列または値。
 * @param rhs 計算に使用する行列または値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator-(Mat lhs, const Mat& rhs) { return lhs -= rhs; }
/**
 * @brief 演算子 operator* で値を扱う。
 * @param lhs 計算に使用する行列または値。
 * @param rhs 計算に使用する行列または値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator*(Mat lhs, const Mat& rhs) { return lhs *= rhs; }
/**
 * @brief 演算子 operator* で値を扱う。
 * @param lhs 計算に使用する行列または値。
 * @param s スケール値または分解結果のスケール。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator*(Mat lhs, float s) { return lhs *= s; }
/**
 * @brief 演算子 operator* で値を扱う。
 * @param s スケール値または分解結果のスケール。
 * @param rhs 計算に使用する行列または値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator*(float s, Mat rhs) { return rhs *= s; }
/**
 * @brief 演算子 operator/ で値を扱う。
 * @param lhs 計算に使用する行列または値。
 * @param s スケール値または分解結果のスケール。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator/(Mat lhs, float s) { return lhs /= s; }

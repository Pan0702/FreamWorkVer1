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
     * @brief インスタンスの初期状態を整える。
     */
    Mat() = default;

    /**
     * @brief インスタンスの初期状態を整える。
     * @param other other に設定する値。
     */
    Mat(const XMMATRIX& other)
    {
        XMStoreFloat4x4(this, other);
    }

    /**
     * @brief インスタンスの初期状態を整える。
     * @param in_11 in_11 に設定する値。
     * @param in_12 in_12 に設定する値。
     * @param in_13 in_13 に設定する値。
     * @param in_14 in_14 に設定する値。
     * @param in_21 in_21 に設定する値。
     * @param in_22 in_22 に設定する値。
     * @param in_23 in_23 に設定する値。
     * @param in_24 in_24 に設定する値。
     * @param in_31 in_31 に設定する値。
     * @param in_32 in_32 に設定する値。
     * @param in_33 in_33 に設定する値。
     * @param in_34 in_34 に設定する値。
     * @param in_41 in_41 に設定する値。
     * @param in_42 in_42 に設定する値。
     * @param in_43 in_43 に設定する値。
     * @param in_44 in_44 に設定する値。
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
     * @brief インスタンスの初期状態を整える。
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
     * @param s s に設定する値。
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
     * @param s s に設定する値。
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
 * @param s s に設定する値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator*(Mat lhs, float s) { return lhs *= s; }
/**
 * @brief 演算子 operator* で値を扱う。
 * @param s s に設定する値。
 * @param rhs 計算に使用する行列または値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator*(float s, Mat rhs) { return rhs *= s; }
/**
 * @brief 演算子 operator/ で値を扱う。
 * @param lhs 計算に使用する行列または値。
 * @param s s に設定する値。
 * @return 演算結果として作成した新しい値。
 */
inline Mat operator/(Mat lhs, float s) { return lhs /= s; }

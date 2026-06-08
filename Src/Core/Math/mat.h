#pragma once

#include <DirectXMath.h>

#include "vec3.h"
using namespace DirectX;

/**
 * @brief Matのデータと処理をまとめる型。
 */
struct Mat : public XMFLOAT4X4
{
    Mat() = default;

    /**
     * @brief Matを初期化するコンストラクタ。
     * @param other 引数。
     */
    Mat(const XMMATRIX& other)
    {
        XMStoreFloat4x4(this, other);
    }

    /**
     * @brief Matを初期化するコンストラクタ。
     * @param in_11 引数。
     * @param in_12 引数。
     * @param in_13 引数。
     * @param in_14 引数。
     * @param in_21 引数。
     * @param in_22 引数。
     * @param in_23 引数。
     * @param in_24 引数。
     * @param in_31 引数。
     * @param in_32 引数。
     * @param in_33 引数。
     * @param in_34 引数。
     * @param in_41 引数。
     * @param in_42 引数。
     * @param in_43 引数。
     * @param in_44 引数。
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

    // 戻り値: DirectXMath の計算用行列。
    operator XMMATRIX() const
    {
        return XMLoadFloat4x4((this));
    }

    // 四則演算。
    Mat& operator+=(const Mat& rhs)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += rhs.m[i][j];
        return *this;
    }

    Mat& operator-=(const Mat& rhs)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= rhs.m[i][j];
        return *this;
    }

    Mat& operator*=(const Mat& rhs)
    {
        *this = static_cast<Mat>(static_cast<XMMATRIX>(*this) * (rhs));
        return *this;
    }

    Mat& operator*=(float s)
    {
        for (auto& i : m)
            for (float& j : i)
                j *= s;
        return *this;
    }

    Mat& operator/=(float s)
    {
        const float inv = 1.0f / s;
        for (auto& i : m)
            for (float& j : i)
                j *= inv;
        return *this;
    }
};

// 四則演算。
inline Mat operator+(Mat lhs, const Mat& rhs) { return lhs += rhs; }
// 四則演算。
inline Mat operator-(Mat lhs, const Mat& rhs) { return lhs -= rhs; }
// 四則演算。
inline Mat operator*(Mat lhs, const Mat& rhs) { return lhs *= rhs; }
// 四則演算。
inline Mat operator*(Mat lhs, float s) { return lhs *= s; }
// 四則演算。
inline Mat operator*(float s, Mat rhs) { return rhs *= s; }
// 四則演算。
inline Mat operator/(Mat lhs, float s) { return lhs /= s; }

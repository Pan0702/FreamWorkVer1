#pragma once

#include <DirectXMath.h>
using namespace DirectX;

struct MatSimd : public XMMATRIX
{
    using XMMATRIX::XMMATRIX;
    MatSimd() = default;
    MatSimd(const XMMATRIX& other) : XMMATRIX(other) {}
};

struct Mat
{
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };

    Mat() = default;

    Mat(const XMMATRIX& other)
    {
        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(this), other);
    }

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

    operator XMMATRIX() const
    {
        return XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this));
    }

    explicit operator MatSimd() const
    {
        return XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(this));
    }

    // ===== 複合代入演算子 =====
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
        *this = static_cast<Mat>(static_cast<XMMATRIX>(*this) * static_cast<XMMATRIX>(rhs));
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

// ===== 二項演算子 =====
inline Mat operator+(Mat lhs, const Mat& rhs) { return lhs += rhs; }
inline Mat operator-(Mat lhs, const Mat& rhs) { return lhs -= rhs; }
inline Mat operator*(Mat lhs, const Mat& rhs) { return lhs *= rhs; }
inline Mat operator*(Mat lhs, float s) { return lhs *= s; }
inline Mat operator*(float s, Mat rhs) { return rhs *= s; }
inline Mat operator/(Mat lhs, float s) { return lhs /= s; }






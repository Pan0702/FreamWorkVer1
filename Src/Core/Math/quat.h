#pragma once
#include <DirectXMath.h>
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

/**
 * @brief Quatのデータと処理をまとめる型。
 */
struct Quat : public XMFLOAT4
{
    /**
     * @brief Quatを初期化するコンストラクタ。
     * @param f 引数。
     */
    Quat() : XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f){}
    /**
     * @brief Quatを初期化するコンストラクタ。
     * @param x 引数。
     * @param y 引数。
     * @param z 引数。
     * @param w 引数。
     */
    Quat(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w){}

    /**
     * @brief Quatを初期化するコンストラクタ。
     * @param v 引数。
     */
    Quat(const XMVECTOR& v)
    {
        XMStoreFloat4((this), v);
    }

    Quat& operator=(const Quat&) = default;
    // v: DirectXMath のベクトル。戻り値: 代入後の自分自身。
    Quat& operator=(const XMVECTOR& v)
    {
        XMStoreFloat4((this), v);
        return *this;
    }

    // 戻り値: DirectXMath の計算用ベクトル。
    operator XMVECTOR() const
    {
        return XMLoadFloat4((this));
    }

    // 四則演算。
    Quat operator*(const Quat& rhs) const
    {
        return Quat(XMQuaternionMultiply(*this, rhs));
    }
    Quat& operator*=(const Quat& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    // 比較。
    bool operator==(const Quat& rhs) const
    {
        return XMQuaternionEqual(*this, rhs);
    }

    bool operator!=(const Quat& rhs) const
    {
        return !(*this == rhs);
    }

    // 戻り値: 長さの二乗。
    float LengthSquared() const
    {
        return XMVectorGetX(XMQuaternionLengthSq(*this));
    }

    // 戻り値: 長さ。
    float Length() const
    {
        return std::sqrt(LengthSquared());
    }
    // 戻り値: 正規化したクォータニオン。
    Quat Normalized() const
    {
        return Quat(XMQuaternionNormalize(*this));
    }
    /**
     * @brief Normalizeを行う関数。
     * @param this 引数。
     */
    void Normalize() { *this = Normalized(); }
    // 戻り値: 共役クォータニオン。
    Quat Conjugate() const
    {
        return Quat(XMQuaternionConjugate(*this));
    }
};

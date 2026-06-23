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
     * @brief インスタンスの初期状態を整える。
     */
    Quat() : XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f){}
    /**
     * @brief インスタンスの初期状態を整える。
     * @param x x に設定する値。
     * @param y y に設定する値。
     * @param z z に設定する値。
     * @param y y に設定する値。
     * @param z z に設定する値。
     * @param w w に設定する値。
     */
    Quat(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w){}

    /**
     * @brief インスタンスの初期状態を整える。
     * @param v 計算に使用するベクトルまたは点。
     */
    Quat(const XMVECTOR& v)
    {
        XMStoreFloat4((this), v);
    }

    /**
     * @brief 演算子 operator= で値を扱う。
     * @return 演算結果を反映した自分自身。
     */
    Quat& operator=(const Quat&) = default;
    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Quat& operator=(const XMVECTOR& v)
    {
        XMStoreFloat4((this), v);
        return *this;
    }

    /**
     * @brief インスタンスの初期状態を整える。
     */
    operator XMVECTOR() const
    {
        return XMLoadFloat4((this));
    }

    /**
     * @brief 演算子 operator* で値を扱う。
     * @param rhs 計算に使用する行列または値。
     * @return 演算結果として作成した新しい値。
     */
    Quat operator*(const Quat& rhs) const
    {
        return Quat(XMQuaternionMultiply(*this, rhs));
    }
    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param rhs 計算に使用する行列または値。
     * @return 演算結果を反映した自分自身。
     */
    Quat& operator*=(const Quat& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    bool operator==(const Quat& rhs) const
    {
        return XMQuaternionEqual(*this, rhs);
    }

    /**
     * @brief 演算子 operator!= で値を扱う。
     * @param rhs 計算に使用する行列または値。
     * @return 左右の値が異なる場合は true。
     */
    bool operator!=(const Quat& rhs) const
    {
        return !(*this == rhs);
    }

    /**
     * @brief クォータニオンの大きさの二乗を求める。
     * @return 平方根を取らないクォータニオンの大きさの二乗。
     */
    float LengthSquared() const
    {
        return XMVectorGetX(XMQuaternionLengthSq(*this));
    }

    /**
     * @brief クォータニオンの大きさを求める。
     * @return クォータニオンの大きさ。
     */
    float Length() const
    {
        return std::sqrt(LengthSquared());
    }
    /**
     * @brief 正規化したクォータニオンを作成する。
     * @return 長さを 1 にそろえたクォータニオン。
     */
    Quat Normalized() const
    {
        return Quat(XMQuaternionNormalize(*this));
    }
    /**
     * @brief 数学計算の結果を求める。
     * @param this this に設定する値。
     */
    void Normalize() { *this = Normalized(); }
    /**
     * @brief 逆向きの回転を作るための共役を求める。
     * @return x・y・z の符号を反転した共役クォータニオン。
     */
    Quat Conjugate() const
    {
        return Quat(XMQuaternionConjugate(*this));
    }
};

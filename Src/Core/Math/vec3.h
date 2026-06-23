#pragma once
#include <cmath>
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

/**
 * @brief Vec3のデータと処理をまとめる型。
 */
struct Vec3 : public XMFLOAT3
{
    /**
     * @brief インスタンスの初期状態を整える。
     */
    Vec3() : XMFLOAT3(0.0f, 0.0f, 0.0f)
    {
    }

    /**
     * @brief インスタンスの初期状態を整える。
     * @param x x に設定する値。
     * @param y y に設定する値。
     * @param y y に設定する値。
     * @param z z に設定する値。
     */
    Vec3(float x, float y, float z) : XMFLOAT3(x, y, z)
    {
    }
    /**
     * @brief インスタンスの初期状態を整える。
     * @param v 計算に使用するベクトルまたは点。
     */
    Vec3(const XMVECTOR& v)
    {
        XMStoreFloat3((this), v);
    }

    /**
     * @brief 演算子 operator+ で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
    /**
     * @brief 演算子 operator+ で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator+(float n) const { return Vec3(x + n, y + n, z + n); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator-(float n) const { return Vec3(x - n, y - n, z - n); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator*(float n) const { return Vec3(x * n, y * n, z * n); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @param n n に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator/(float n) const { return Vec3(x / n, y / n, z / n); }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator-=(const Vec3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator*=(const Vec3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator/=(const Vec3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator+=(float n)
    {
        x += n;
        y += n;
        z += n;
        return *this;
    }

    /**
     * @brief 演算子 operator-= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator-=(float n)
    {
        x -= n;
        y -= n;
        z -= n;
        return *this;
    }

    /**
     * @brief 演算子 operator*= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator*=(float n)
    {
        x *= n;
        y *= n;
        z *= n;
        return *this;
    }

    /**
     * @brief 演算子 operator/= で値を扱う。
     * @param n n に設定する値。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator/=(float n)
    {
        x /= n;
        y /= n;
        z /= n;
        return *this;
    }

    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
    /**
     * @brief 演算子 operator!= で値を扱う。
     * @return 左右の値が異なる場合は true。
     */
    bool operator!=(const Vec3& v) const { return !(*this == v); }

    /**
     * @brief 演算子 operator+ で値を扱う。
     * @param y y に設定する値。
     * @param z z に設定する値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator+() const { return Vec3(x, y, z); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator=(const Vec3& v) = default;

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 計算に使用するベクトルまたは点。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator=(const XMVECTOR& v)
    {
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(this), v);
        return *this;
    }

    /**
     * @brief インスタンスの初期状態を整える。
     */
    operator XMVECTOR() const
    {
        
        return XMLoadFloat3((this));
    }

    /**
     * @brief インスタンスの初期状態を整える。
     */
    operator XMFLOAT3() const
    {
        return {x, y, z};
    }

    /**
     * @brief ベクトルの長さの二乗を求める。
     * @return 平方根を取らない Vec3 の長さの二乗。
     */
    float LengthSquared() const { return x * x + y * y + z * z; }
    /**
     * @brief ベクトルの長さを求める。
     * @return Vec3 の長さ。
     */
    float Length() const { return std::sqrt(LengthSquared()); }

    /**
     * @brief 数学計算の結果を求める。
     * @return 長さを 1 にそろえた Vec3。
     */
    Vec3 Normalized() const
    {
        float len = Length();
        return len > 0 ? Vec3(x / len, y / len, z / len) : Vec3(0, 0, 0);
    }
    /**
     * @brief 数学計算の結果を求める。
     * @param this this に設定する値。
     */
    void Normalize() { *this = Normalized(); }
};
/**
 * @brief 演算子 operator* で値を扱う。
 * @param n n に設定する値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 演算結果として作成した新しい値。
 */
inline Vec3 operator*(float n, const Vec3& v) { return v * n; }

/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @return 2 つのベクトルの内積。
 */
inline float Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @return 2 つのベクトルの外積。
 */
inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @return 保持している Vec3 値 への参照。
 */
inline float DistanceSquared(const Vec3& a, const Vec3& b) { return (a - b).LengthSquared(); }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @return ベクトルの長さ。
 */
inline float Distance(const Vec3& a, const Vec3& b) { return (a - b).Length(); }
/**
 * @brief 数学計算の結果を求める。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param a 計算に使用するベクトルまたは点。
 * @return a から b へ t だけ線形補間した値。
 */
inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t) { return a + (b - a) * t; }
/**
 * @brief 現在の状態が条件を満たしているか調べる。
 * @param v 計算に使用するベクトルまたは点。
 */
inline void IsZeroVec(Vec3& v)
{
    if ( XMVector3Equal(XMLoadFloat3(&v), XMVectorZero()))
    {
         v = Vec3(0,0,-2.0f);
    }
}

/**
 * @brief 数学計算の結果を求める。
 * @param v 計算に使用するベクトルまたは点。
 * @return 各成分を絶対値にした値。
 */
inline Vec3 Abs(const Vec3& v)
{
    return Vec3(std::fabs(v.x), std::fabs(v.y), std::fabs(v.z));
}

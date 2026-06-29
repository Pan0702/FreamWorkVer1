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
     * @brief 値を初期化する。
     */
    Vec3() : XMFLOAT3(0.0f, 0.0f, 0.0f)
    {
    }

    /**
     * @brief 値を初期化する。
     * @param x X 成分。
     * @param y Y 成分。
     * @param y Y 成分。
     * @param z Z 成分。
     */
    Vec3(float x, float y, float z) : XMFLOAT3(x, y, z)
    {
    }
    /**
     * @brief 値を初期化する。
     * @param v 演算に使うベクトル。
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
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator+(float n) const { return Vec3(x + n, y + n, z + n); }
    /**
     * @brief 演算子 operator- で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator-(float n) const { return Vec3(x - n, y - n, z - n); }
    /**
     * @brief 演算子 operator* で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator*(float n) const { return Vec3(x * n, y * n, z * n); }
    /**
     * @brief 演算子 operator/ で値を扱う。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @param n 各成分に適用するスカラー値。
     * @return 演算結果として作成した新しい値。
     */
    Vec3 operator/(float n) const { return Vec3(x / n, y / n, z / n); }

    /**
     * @brief 演算子 operator+= で値を扱う。
     * @param v 演算に使うベクトル。
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
     * @param v 演算に使うベクトル。
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
     * @param v 演算に使うベクトル。
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
     * @param v 演算に使うベクトル。
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
     * @param n 各成分に適用するスカラー値。
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
     * @param n 各成分に適用するスカラー値。
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
     * @param n 各成分に適用するスカラー値。
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
     * @param n 各成分に適用するスカラー値。
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
     * @param y Y 成分。
     * @param z Z 成分。
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
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator=(const Vec3& v) = default;

    /**
     * @brief 演算子 operator= で値を扱う。
     * @param v 演算に使うベクトル。
     * @return 演算結果を反映した自分自身。
     */
    Vec3& operator=(const XMVECTOR& v)
    {
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(this), v);
        return *this;
    }

    /**
     * @brief 値を初期化する。
     */
    operator XMVECTOR() const
    {
        
        return XMLoadFloat3((this));
    }

    /**
     * @brief 値を初期化する。
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
     * @brief ベクトルや角度の補助計算を行う。
     * @return 長さを 1 にそろえた Vec3。
     */
    Vec3 Normalized() const
    {
        float len = Length();
        return len > 0 ? Vec3(x / len, y / len, z / len) : Vec3(0, 0, 0);
    }
    /**
     * @brief ベクトルや角度の補助計算を行う。
     */
    void Normalize() { *this = Normalized(); }
};
/**
 * @brief 演算子 operator* で値を扱う。
 * @param n 各成分に適用するスカラー値。
 * @param v 演算に使うベクトル。
 * @return 演算結果として作成した新しい値。
 */
inline Vec3 operator*(float n, const Vec3& v) { return v * n; }

/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @return 2 つのベクトルの内積。
 */
inline float Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @return 2 つのベクトルの外積。
 */
inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @return 2 点間の距離の二乗。
 */
inline float DistanceSquared(const Vec3& a, const Vec3& b) { return (a - b).LengthSquared(); }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @return ベクトルの長さ。
 */
inline float Distance(const Vec3& a, const Vec3& b) { return (a - b).Length(); }
/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param a 1 つ目のベクトルまたは点。
 * @param b 2 つ目のベクトルまたは点。
 * @param a 1 つ目のベクトルまたは点。
 * @return a から b へ t だけ線形補間した値。
 */
inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t) { return a + (b - a) * t; }
/**
 * @brief 現在の状態が条件を満たしているか調べる。
 * @param v 演算に使うベクトル。
 */
inline void IsZeroVec(Vec3& v)
{
    if ( XMVector3Equal(XMLoadFloat3(&v), XMVectorZero()))
    {
         v = Vec3(0,0,-2.0f);
    }
}

/**
 * @brief ベクトルや角度の補助計算を行う。
 * @param v 演算に使うベクトル。
 * @return 各成分を絶対値にした値。
 */
inline Vec3 Abs(const Vec3& v)
{
    return Vec3(std::fabs(v.x), std::fabs(v.y), std::fabs(v.z));
}

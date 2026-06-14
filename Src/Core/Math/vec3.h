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
     * @brief Vec3を初期化するコンストラクタ。
     * @param f 引数。
     */
    Vec3() : XMFLOAT3(0.0f, 0.0f, 0.0f)
    {
    }

    /**
     * @brief Vec3を初期化するコンストラクタ。
     * @param x 引数。
     * @param y 引数。
     * @param z 引数。
     */
    Vec3(float x, float y, float z) : XMFLOAT3(x, y, z)
    {
    }
    /**
     * @brief Vec3を初期化するコンストラクタ。
     * @param v 引数。
     */
    Vec3(const XMVECTOR& v)
    {
        XMStoreFloat3((this), v);
    }

    // 四則演算。
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    // 四則演算。
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    // 四則演算。
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    // 四則演算。
    Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
    // 四則演算。
    Vec3 operator+(float n) const { return Vec3(x + n, y + n, z + n); }
    // 四則演算。
    Vec3 operator-(float n) const { return Vec3(x - n, y - n, z - n); }
    // 四則演算。
    Vec3 operator*(float n) const { return Vec3(x * n, y * n, z * n); }
    // 四則演算。
    Vec3 operator/(float n) const { return Vec3(x / n, y / n, z / n); }

    //複合代入（破壊・自身を参照で返す）
    Vec3& operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(const Vec3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    Vec3& operator/=(const Vec3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    Vec3& operator+=(float n)
    {
        x += n;
        y += n;
        z += n;
        return *this;
    }

    Vec3& operator-=(float n)
    {
        x -= n;
        y -= n;
        z -= n;
        return *this;
    }

    Vec3& operator*=(float n)
    {
        x *= n;
        y *= n;
        z *= n;
        return *this;
    }

    Vec3& operator/=(float n)
    {
        x /= n;
        y /= n;
        z /= n;
        return *this;
    }

    // 四則演算。
    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
    // 四則演算。
    bool operator!=(const Vec3& v) const { return !(*this == v); }

    // 四則演算。
    Vec3 operator+() const { return Vec3(x, y, z); }
    // 四則演算。
    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    //代入
    Vec3& operator=(const Vec3& v) = default;

    Vec3& operator=(const XMVECTOR& v)
    {
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(this), v);
        return *this;
    }

    //変換
    operator XMVECTOR() const
    {
        
        return XMLoadFloat3((this));
    }

    operator XMFLOAT3() const
    {
        return {x, y, z};
    }

    /**
     * @brief LengthSquaredを行う関数。
     * @return 戻り値。
     */
    float LengthSquared() const { return x * x + y * y + z * z; }
    /**
     * @brief Lengthを行う関数。
     * @param LengthSquared 引数。
     * @return 戻り値。
     */
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec3 Normalized() const
    {
        float len = Length();
        return len > 0 ? Vec3(x / len, y / len, z / len) : Vec3(0, 0, 0);
    }
    /**
     * @brief Normalizeを行う関数。
     * @param this 引数。
     */
    void Normalize() { *this = Normalized(); }
};
// 四則演算。
inline Vec3 operator*(float n, const Vec3& v) { return v * n; }

inline float Dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
    return Vec3(a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x);
}
/**
 * @brief DistanceSquaredを行う関数。
 * @param a 引数。
 * @param LengthSquared 引数。
 * @return 戻り値。
 */
inline float DistanceSquared(const Vec3& a, const Vec3& b) { return (a - b).LengthSquared(); }
/**
 * @brief Distanceを行う関数。
 * @param a 引数。
 * @param Length 引数。
 * @return 戻り値。
 */
inline float Distance(const Vec3& a, const Vec3& b) { return (a - b).Length(); }
/**
 * @brief Lerpを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @param t 引数。
 * @return 戻り値。
 */
inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t) { return a + (b - a) * t; }
inline void IsZeroVec(Vec3& v)
{
    if ( XMVector3Equal(XMLoadFloat3(&v), XMVectorZero()))
    {
         v = Vec3(0,0,-2.0f);
    }
}

inline Vec3 Abs(const Vec3& v)
{
    return Vec3(std::fabs(v.x), std::fabs(v.y), std::fabs(v.z));
}

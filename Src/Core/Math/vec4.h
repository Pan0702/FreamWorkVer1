#pragma once
#include <cmath>
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

/**
 * @brief Vec4のデータと処理をまとめる型。
 */
struct Vec4 : public XMFLOAT4
{
    /**
     * @brief Vec4を初期化するコンストラクタ。
     * @param f 引数。
     */
    Vec4() : XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }
    /**
     * @brief Vec4を初期化するコンストラクタ。
     * @param x 引数。
     * @param y 引数。
     * @param z 引数。
     * @param w 引数。
     */
    Vec4(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w)
    {
    }

    /**
     * @brief Vec4を初期化するコンストラクタ。
     * @param v 引数。
     */
    Vec4(const XMVECTOR& v)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), v);
    }

    // 四則演算。
    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    // 四則演算。
    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    // 四則演算。
    Vec4 operator*(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    // 四則演算。
    Vec4 operator/(const Vec4& v) const { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
    // 四則演算。
    Vec4 operator+(float n) const { return Vec4(x + n, y + n, z + n, w + n); }
    // 四則演算。
    Vec4 operator-(float n) const { return Vec4(x - n, y - n, z - n, w - n); }
    // 四則演算。
    Vec4 operator*(float n) const { return Vec4(x * n, y * n, z * n, w * n); }
    // 四則演算。
    Vec4 operator/(float n) const { return Vec4(x / n, y / n, z / n, w / n); }

    // 四則演算。
    Vec4& operator+=(const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    // 四則演算。
    Vec4& operator-=(const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    // 四則演算。
    Vec4& operator*=(const Vec4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
    // 四則演算。
    Vec4& operator/=(const Vec4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
    // 四則演算。
    Vec4& operator+=(float n) { x += n; y += n; z += n; w += n; return *this; }
    // 四則演算。
    Vec4& operator-=(float n) { x -= n; y -= n; z -= n; w -= n; return *this; }
    // 四則演算。
    Vec4& operator*=(float n) { x *= n; y *= n; z *= n; w *= n; return *this; }
    // 四則演算。
    Vec4& operator/=(float n) { x /= n; y /= n; z /= n; w /= n; return *this; }

    // 四則演算。
    bool operator==(const Vec4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    // 四則演算。
    bool operator!=(const Vec4& v) const { return !(*this == v); }

    // 四則演算。
    Vec4 operator+() const { return Vec4(x, y, z, w); }
    // 四則演算。
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

    //代入
    Vec4& operator=(const Vec4& v) = default;

    Vec4& operator=(const XMVECTOR& v)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), v);
        return *this;
    }

    //変換
    operator XMVECTOR() const
    {
        return XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(this));
    }

    operator XMFLOAT4() const
    {
        return {x, y, z, w};
    }

    /**
     * @brief LengthSquaredを行う関数。
     * @return 戻り値。
     */
    float LengthSquared() const { return x * x + y * y + z * z + w * w; }
    /**
     * @brief Lengthを行う関数。
     * @param LengthSquared 引数。
     * @return 戻り値。
     */
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec4 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec4(x / len, y / len, z / len, w / len) : Vec4(0, 0, 0, 0);
    }
    /**
     * @brief Normalizeを行う関数。
     * @param this 引数。
     */
    void Normalize() { *this = Normalized(); }
};
// 四則演算。
inline Vec4 operator*(float n, const Vec4& v) { return v * n; }
/**
 * @brief Dotを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @return 戻り値。
 */
inline float Dot(const Vec4& a, const Vec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
/**
 * @brief DistanceSquaredを行う関数。
 * @param a 引数。
 * @param LengthSquared 引数。
 * @return 戻り値。
 */
inline float DistanceSquared(const Vec4& a, const Vec4& b) { return (a - b).LengthSquared(); }
/**
 * @brief Distanceを行う関数。
 * @param a 引数。
 * @param Length 引数。
 * @return 戻り値。
 */
inline float Distance(const Vec4& a, const Vec4& b) { return (a - b).Length(); }
/**
 * @brief Lerpを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @param t 引数。
 * @return 戻り値。
 */
inline Vec4 Lerp(const Vec4& a, const Vec4& b, float t) { return a + (b - a) * t; }



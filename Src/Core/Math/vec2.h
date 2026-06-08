#pragma once
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

/**
 * @brief Vec2のデータと処理をまとめる型。
 */
struct Vec2 : public XMFLOAT2
{
    /**
     * @brief Vec2を初期化するコンストラクタ。
     * @param f 引数。
     */
    Vec2() : XMFLOAT2(0.0f, 0.0f)
    {
    }
    /**
     * @brief Vec2を初期化するコンストラクタ。
     * @param x 引数。
     * @param y 引数。
     */
    Vec2(float x, float y) : XMFLOAT2(x, y)
    {
    }

    /**
     * @brief Vec2を初期化するコンストラクタ。
     * @param v 引数。
     */
    Vec2(const XMVECTOR& v)
    {
        XMStoreFloat2((this), v);
    }

    // 四則演算。
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    // 四則演算。
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    // 四則演算。
    Vec2 operator*(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    // 四則演算。
    Vec2 operator/(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
    // 四則演算。
    Vec2 operator+(float n) const { return Vec2(x + n, y + n); }
    // 四則演算。
    Vec2 operator-(float n) const { return Vec2(x - n, y - n); }
    // 四則演算。
    Vec2 operator*(float n) const { return Vec2(x * n, y * n); }
    // 四則演算。
    Vec2 operator/(float n) const { return Vec2(x / n, y / n); }

    // 四則演算。
    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    // 四則演算。
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    // 四則演算。
    Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
    // 四則演算。
    Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }
    // 四則演算。
    Vec2& operator+=(float n) { x += n; y += n; return *this; }
    // 四則演算。
    Vec2& operator-=(float n) { x -= n; y -= n; return *this; }
    // 四則演算。
    Vec2& operator*=(float n) { x *= n; y *= n; return *this; }
    // 四則演算。
    Vec2& operator/=(float n) { x /= n; y /= n; return *this; }

    // 四則演算。
    bool operator==(const Vec2& v) const { return x == v.x && y == v.y; }
    // 四則演算。
    bool operator!=(const Vec2& v) const { return !(*this == v); }

    // 四則演算。
    Vec2 operator+() const { return Vec2(x, y); }
    // 四則演算。
    Vec2 operator-() const { return Vec2(-x, -y); }

    //代入
    Vec2& operator=(const Vec2& v) = default;

    Vec2& operator=(const XMVECTOR& v)
    {
        XMStoreFloat2((this), v);
        return *this;
    }

    //変換
    explicit operator XMVECTOR() const
    {
        return XMLoadFloat2((this));
    }

    operator XMFLOAT2() const
    {
        return {x, y};
    }

    /**
     * @brief LengthSquaredを行う関数。
     * @return 戻り値。
     */
    float LengthSquared() const { return x * x + y * y; }
    /**
     * @brief Lengthを行う関数。
     * @param LengthSquared 引数。
     * @return 戻り値。
     */
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec2 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec2(x / len, y / len) : Vec2(0, 0);
    }
    /**
     * @brief Normalizeを行う関数。
     * @param this 引数。
     */
    void Normalize() { *this = Normalized(); }
};
// 四則演算。
inline Vec2 operator*(float n, const Vec2& v) { return v * n; }
/**
 * @brief Dotを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @return 戻り値。
 */
inline float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
/**
 * @brief Crossを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @return 戻り値。
 */
inline float Cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }
/**
 * @brief DistanceSquaredを行う関数。
 * @param a 引数。
 * @param LengthSquared 引数。
 * @return 戻り値。
 */
inline float DistanceSquared(const Vec2& a, const Vec2& b) { return (a - b).LengthSquared(); }
/**
 * @brief Distanceを行う関数。
 * @param a 引数。
 * @param Length 引数。
 * @return 戻り値。
 */
inline float Distance(const Vec2& a, const Vec2& b) { return (a - b).Length(); }
/**
 * @brief Lerpを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @param t 引数。
 * @return 戻り値。
 */
inline Vec2 Lerp(const Vec2& a, const Vec2& b, float t) { return a + (b - a) * t; }



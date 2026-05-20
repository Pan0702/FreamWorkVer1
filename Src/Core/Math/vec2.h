#pragma once
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

struct Vec2
{
    float x, y;

    //初期化
    Vec2(float x, float y) : x(x), y(y)
    {
    }

    Vec2() : x(0), y(0)
    {
    }

    Vec2(const XMVECTOR& v)
    {
        XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(this), v);
    }

    //四則演算（非破壊・const）
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    Vec2 operator/(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }

    Vec2 operator+(float n) const { return Vec2(x + n, y + n); }
    Vec2 operator-(float n) const { return Vec2(x - n, y - n); }
    Vec2 operator*(float n) const { return Vec2(x * n, y * n); }
    Vec2 operator/(float n) const { return Vec2(x / n, y / n); }

    //複合代入（破壊・自身を参照で返す）
    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    Vec2& operator*=(const Vec2& v) { x *= v.x; y *= v.y; return *this; }
    Vec2& operator/=(const Vec2& v) { x /= v.x; y /= v.y; return *this; }

    Vec2& operator+=(float n) { x += n; y += n; return *this; }
    Vec2& operator-=(float n) { x -= n; y -= n; return *this; }
    Vec2& operator*=(float n) { x *= n; y *= n; return *this; }
    Vec2& operator/=(float n) { x /= n; y /= n; return *this; }

    //比較
    bool operator==(const Vec2& v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vec2& v) const { return !(*this == v); }

    //単項
    Vec2 operator+() const { return Vec2(x, y); }
    Vec2 operator-() const { return Vec2(-x, -y); }

    //代入
    Vec2& operator=(const Vec2& v) = default;

    Vec2& operator=(const XMVECTOR& v)
    {
        XMStoreFloat2(reinterpret_cast<XMFLOAT2*>(this), v);
        return *this;
    }

    //変換
    operator XMVECTOR() const
    {
        return XMLoadFloat2(reinterpret_cast<const XMFLOAT2*>(this));
    }

    operator XMFLOAT2() const
    {
        return {x, y};
    }

    //長さ・正規化
    float LengthSquared() const { return x * x + y * y; }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec2 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec2(x / len, y / len) : Vec2(0, 0);
    }

    void Normalize() { *this = Normalized(); }
};

inline Vec2 operator*(float n, const Vec2& v) { return v * n; }

inline float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
inline float Cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }

inline float DistanceSquared(const Vec2& a, const Vec2& b) { return (a - b).LengthSquared(); }
inline float Distance(const Vec2& a, const Vec2& b) { return (a - b).Length(); }

inline Vec2 Lerp(const Vec2& a, const Vec2& b, float t) { return a + (b - a) * t; }

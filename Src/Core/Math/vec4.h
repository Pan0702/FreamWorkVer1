#pragma once
#include <cmath>
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

struct Vec4 : public XMFLOAT4
{
    Vec4() : XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }

    Vec4(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w)
    {
    }

    //初期化
    
    Vec4(const XMVECTOR& v)
    {
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(this), v);
    }

    //四則演算（非破壊・const）
    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vec4 operator*(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    Vec4 operator/(const Vec4& v) const { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }

    Vec4 operator+(float n) const { return Vec4(x + n, y + n, z + n, w + n); }
    Vec4 operator-(float n) const { return Vec4(x - n, y - n, z - n, w - n); }
    Vec4 operator*(float n) const { return Vec4(x * n, y * n, z * n, w * n); }
    Vec4 operator/(float n) const { return Vec4(x / n, y / n, z / n, w / n); }

    //複合代入（破壊・自身を参照で返す）
    Vec4& operator+=(const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    Vec4& operator-=(const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    Vec4& operator*=(const Vec4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
    Vec4& operator/=(const Vec4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

    Vec4& operator+=(float n) { x += n; y += n; z += n; w += n; return *this; }
    Vec4& operator-=(float n) { x -= n; y -= n; z -= n; w -= n; return *this; }
    Vec4& operator*=(float n) { x *= n; y *= n; z *= n; w *= n; return *this; }
    Vec4& operator/=(float n) { x /= n; y /= n; z /= n; w /= n; return *this; }

    //比較
    bool operator==(const Vec4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool operator!=(const Vec4& v) const { return !(*this == v); }

    //単項
    Vec4 operator+() const { return Vec4(x, y, z, w); }
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

    //長さ・正規化
    float LengthSquared() const { return x * x + y * y + z * z + w * w; }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec4 Normalized() const
    {
        float len = Length();
        return (len > 0) ? Vec4(x / len, y / len, z / len, w / len) : Vec4(0, 0, 0, 0);
    }

    void Normalize() { *this = Normalized(); }
};

inline Vec4 operator*(float n, const Vec4& v) { return v * n; }
inline float Dot(const Vec4& a, const Vec4& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
inline float DistanceSquared(const Vec4& a, const Vec4& b) { return (a - b).LengthSquared(); }
inline float Distance(const Vec4& a, const Vec4& b) { return (a - b).Length(); }
inline Vec4 Lerp(const Vec4& a, const Vec4& b, float t) { return a + (b - a) * t; }

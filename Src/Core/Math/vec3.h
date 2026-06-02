#pragma once
#include <cmath>
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

struct Vec3 : public XMFLOAT3
{
    Vec3() : XMFLOAT3(0.0f, 0.0f, 0.0f)
    {
    }

    //初期化
    Vec3(float x, float y, float z) : XMFLOAT3(x, y, z)
    {
    }
    

    Vec3(const XMVECTOR& v)
    {
        XMStoreFloat3((this), v);
    }

    //四則演算（非破壊・const）
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }

    Vec3 operator+(float n) const { return Vec3(x + n, y + n, z + n); }
    Vec3 operator-(float n) const { return Vec3(x - n, y - n, z - n); }
    Vec3 operator*(float n) const { return Vec3(x * n, y * n, z * n); }
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

    //比較
    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec3& v) const { return !(*this == v); }

    //単項
    Vec3 operator+() const { return Vec3(x, y, z); }
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

    //長さ・正規化
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vec3 Normalized() const
    {
        float len = Length();
        return len > 0 ? Vec3(x / len, y / len, z / len) : Vec3(0, 0, 0);
    }

    void Normalize() { *this = Normalized(); }
};

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

inline float DistanceSquared(const Vec3& a, const Vec3& b) { return (a - b).LengthSquared(); }
inline float Distance(const Vec3& a, const Vec3& b) { return (a - b).Length(); }

inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t) { return a + (b - a) * t; }
inline void IsZeroVec(Vec3& v)
{
    if ( XMVector3Equal(XMLoadFloat3(&v), XMVectorZero()))
    {
         v = Vec3(0,0,-2.0f);
    }
}

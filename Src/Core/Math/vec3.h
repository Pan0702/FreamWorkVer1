#pragma once
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

struct Vec3
{
    float x, y ,z;

    void XMStoreFloat3(Vec3* vec2, __m128 v);
    __m128 XMLoadFloat3(const Vec3* vec2) const;
    
    //初期化
    Vec3(float x, float y,float z) : x(x), y(y), z(z){}
    Vec3() : x(0), y(0), z(0){}
    Vec3(const XMVECTOR& v)
    {
        XMVECTOR v2 = v;
        XMStoreFloat3(this, v2);
    }

    //四則演算
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator+=(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator+(float n) const { return Vec3(x + n, y + n, z + n); }

    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator-=(const Vec3& v) const { return Vec3(x - v.x, y - v.y,z - v.z); }
    Vec3 operator-(float n) const { return Vec3(x - n, y - n, z - n); }

    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator*=(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator*(float n) const { return Vec3(x * n, y * n, z * n); }

    Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
    Vec3 operator/=(const Vec3& v) const { return Vec3(x / v.x, y / v.y,z / v.z); }
    Vec3 operator/(float n) const { return Vec3(x / n, y / n, z / n); }

    //
    bool operator==(const Vec3& v) const { return (x == v.x && y == v.y && z == v.z); }
    bool operator!=(const Vec3& v) const { return (x != v.x && y != v.y && z != v.z); }
    bool operator<(const Vec3& v) const { return (x < v.x && y < v.y && z < v.z); }
    bool operator<=(const Vec3& v) const { return (x <= v.x && y <= v.y && z <= v.z); }
    bool operator>=(const Vec3& v) const { return (x >= v.x && y >= v.y && z <= v.z); }
    bool operator>(const Vec3& v) const { return (x > v.x && y > v.y && z > v.z); }
    
    Vec3 operator + () const { return Vec3(x, y, z);}
    Vec3 operator - () const { return Vec3(-x, -y, -z); }
    
    Vec3& operator=(const Vec3& v) {  x = v.x, y = v.y,z = v.z; return *this;}
    Vec3& operator=(const XMVECTOR& v)
    {
        XMVECTOR v2 = v;
        XMStoreFloat3(this, v2);
        return *this;
    }
    
    operator XMVECTOR() const
    {
        return XMLoadFloat3(this);
    }
    operator XMFLOAT3() const
    {
        return {x,y,z};
    }
};

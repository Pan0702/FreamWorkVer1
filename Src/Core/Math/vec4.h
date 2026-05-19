#pragma once
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

struct Vec4
{
    float x, y ,z ,w;

    void XMStoreFloat4(Vec4* vec2, __m128 v);
    __m128 XMLoadFloat4(const Vec4* vec2) const;
    
    //初期化
    Vec4(float x_, float y_,float z_,float w_) : x(x_), y(y_), z(z_),w(w_){}
    Vec4() : x(0), y(0), z(0), w(0){}
    Vec4(const XMVECTOR& v)
    {
        XMVECTOR v2 = v;
        XMStoreFloat4(this, v2);
    }

    //四則演算
    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vec4 operator+=(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vec4 operator+(float n) const { return Vec4(x + n, y + n, z + n, w + n); }

    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vec4 operator-=(const Vec4& v) const { return Vec4(x - v.x, y - v.y,z - v.z, w - v.w); }
    Vec4 operator-(float n) const { return Vec4(x - n, y - n, z - n, w - n); }

    Vec4 operator*(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    Vec4 operator*=(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z,w * v.w); }
    Vec4 operator*(float n) const { return Vec4(x * n, y * n, z * n,w * n); }

    Vec4 operator/(const Vec4& v) const { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
    Vec4 operator/=(const Vec4& v) const { return Vec4(x / v.x, y / v.y,z / v.z,w / v.w); }
    Vec4 operator/(float n) const { return Vec4(x / n, y / n, z / n, w / n); }

    //
    bool operator==(const Vec4& v) const { return (x == v.x && y == v.y && z == v.z && w == v.w); }
    bool operator!=(const Vec4& v) const { return (x != v.x && y != v.y && z != v.z && w != v.w); }
    bool operator<(const Vec4& v) const { return (x < v.x && y < v.y && z < v.z && w < v.w); }
    bool operator<=(const Vec4& v) const { return (x <= v.x && y <= v.y && z <= v.z && w <= v.w); }
    bool operator>=(const Vec4& v) const { return (x >= v.x && y >= v.y && z <= v.z && w >= v.w); }
    bool operator>(const Vec4& v) const { return (x > v.x && y > v.y && z > v.z && w > v.w); }
    
    Vec4 operator + () const { return Vec4(x, y, z, w);}
    Vec4 operator - () const { return Vec4(-x, -y, -z, -w); }
    
    Vec4& operator=(const Vec4& v) {  x = v.x, y = v.y,z = v.z; return *this;}
    Vec4& operator=(const XMVECTOR& v)
    {
        XMVECTOR v2 = v;
        XMStoreFloat4(this, v2);
        return *this;
    }
    
    operator XMVECTOR() const
    {
        return XMLoadFloat4(this);
    }
    operator XMFLOAT4() const
    {
        return {x,y,z,w};
    }
};

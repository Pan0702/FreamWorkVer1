#pragma once
#include <DirectXMath.h>
#include <windows.h>
using namespace DirectX;

struct Vec2
{
    float x, y;

    void XMStoreFloat2(Vec2* vec2, __m128 v);
    __m128 XMLoadFloat2(const Vec2* vec2) const;
    //初期化
    Vec2(float x, float y) : x(x), y(y){}
    Vec2() : x(0), y(0){}
    Vec2(const XMVECTOR& v)
    {
        XMVECTOR v2 = v;
        XMStoreFloat2(this, v2);
    }

    //四則演算
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator+=(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator+(float n) const { return Vec2(x + n, y + n); }

    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator-=(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator-(float n) const { return Vec2(x - n, y - n); }

    Vec2 operator*(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    Vec2 operator*=(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    Vec2 operator*(float n) const { return Vec2(x * n, y * n); }

    Vec2 operator/(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
    Vec2 operator/=(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
    Vec2 operator/(float n) const { return Vec2(x / n, y / n); }

    //
    bool operator==(const Vec2& v) const { return (x == v.x && y == v.y); }
    bool operator!=(const Vec2& v) const { return (x != v.x && y != v.y); }
    bool operator<(const Vec2& v) const { return (x < v.x && y < v.y); }
    bool operator<=(const Vec2& v) const { return (x <= v.x && y <= v.y); }
    bool operator>=(const Vec2& v) const { return (x >= v.x && y >= v.y); }
    bool operator>(const Vec2& v) const { return (x > v.x && y > v.y); }
    
    Vec2 operator + () const { return Vec2(x, y);}
    Vec2 operator - () const { return Vec2(-x, -y); }
    
    Vec2& operator=(const Vec2& v){ x = v.x; y = v.y; return *this;}

    Vec2& operator=(const XMVECTOR& v)
    {
        XMVECTOR v2 = v;
        XMStoreFloat2(this, v2);
        return *this;
    }
    
    operator XMVECTOR() const
    {
        return XMLoadFloat2(this);
    }
    operator XMFLOAT2() const
    {
        return {x,y};
    }
};

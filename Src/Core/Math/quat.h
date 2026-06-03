#pragma once
#include <DirectXMath.h>
#include <DirectXMath.h>
#include <cmath>
using namespace DirectX;

struct Quat : public XMFLOAT4
{
    Quat() : XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f){}
    Quat(float x, float y, float z, float w) : XMFLOAT4(x, y, z, w){}
    
    Quat(const XMVECTOR& v)
    {
        XMStoreFloat4((this), v);
    }
    
    Quat& operator=(const Quat&) = default;
    Quat& operator=(const XMVECTOR& v)
    {
        XMStoreFloat4((this), v);
        return *this;
    }
    
    operator XMVECTOR() const
    {
        return XMLoadFloat4((this));
    }
    
    Quat operator*(const Quat& rhs) const
    {
        return Quat(XMQuaternionMultiply(*this,rhs));
    }
    Quat& operator*=(const Quat& rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    
    bool operator==(const Quat& rhs) const
    {
        return XMQuaternionEqual(*this, rhs);
    }
    
    bool operator!=(const Quat& rhs) const
    {
        return !(*this == rhs);
    }
    
    float LengthSquared() const
    {
        return XMVectorGetX(XMQuaternionLengthSq(*this));
    }
    
    float Length() const
    {
        return std::sqrt(LengthSquared());
    }
    Quat Normalized() const
    {
        return Quat(XMQuaternionNormalize(*this));
    }
    void Normalize() { *this = Normalized(); }
    Quat Conjugate() const
    {
        return Quat(XMQuaternionConjugate(*this));
    }
};
    
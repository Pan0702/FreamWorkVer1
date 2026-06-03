#include "my_math.h"

Mat Identity()
{
    return XMMatrixIdentity();
}

Mat Transpose(const Mat& m)
{
    return XMMatrixTranspose(m);
}

Mat Inverse(const Mat& m)
{
    return XMMatrixInverse(nullptr, m);   
}

Mat RotateX(float rot)
{
    return XMMatrixRotationX(rot);
}

Mat RotateY(float rot)
{
    return XMMatrixRotationY(rot);
}

Mat RotateZ(float rot)
{
    return XMMatrixRotationZ(rot);  
}

Mat Scale(const Vec3& scale)
{
    return XMMatrixScaling(scale.x, scale.y, scale.z); 
}

Mat Translate(const Vec3& trans)
{
    return XMMatrixTranslation(trans.x, trans.y, trans.z);
}

Mat LookAtLH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up)
{
    return XMMatrixLookAtLH(eye, at, up);
}

Mat LookAtRH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up)
{
    return XMMatrixLookAtRH(eye, at, up);
}

Mat PerspectiveFovLH(float fov, float aspect, float zNear, float zFar)
{
    return XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
}

Quat QuatIdentity()
{
    return Quat(XMQuaternionIdentity());
}

Quat QuatFromAxisAngle(const Vec3& axis, float angle)
{
    return Quat(XMQuaternionRotationAxis(axis, angle));
}

Quat QuatFromEuler(const Vec3& euler)
{
    return Quat(XMQuaternionRotationRollPitchYaw(euler.x, euler.y, euler.z));
}

Quat Slerp(const Quat& a, const Quat& b, float t)
{
    return Quat(XMQuaternionSlerp(a, b, t));
}

Mat ToMat(const Quat& q)
{
    return XMMatrixRotationQuaternion(q);
}

Quat ToQuat(const Mat& m)
{
    return Quat(XMQuaternionRotationMatrix(m));
}

Vec3 Rotate(const Quat& q, const Vec3& v)
{
    return Vec3(XMVector3Rotate(v, q));
}

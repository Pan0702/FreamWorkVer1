#include "my_math.h"

#include <cmath>

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

Mat OrthographicLH(float ViewWidth, float ViewHeight, float zNear, float zFar)
{
    return XMMatrixOrthographicLH(ViewWidth, ViewHeight, zNear, zFar);
}

Vec3 TransformDirection(const Mat& m, const Vec3& v)
{
    return Vec3(XMVector3TransformNormal(v, m));
}

Vec3 TransformPoint(const Mat& m, const Vec3& v)
{
    return Vec3(XMVector3Transform(v, m));
}

Vec3 TransformNormal(const Mat& m, const Vec3& v)
{
    return Vec3(XMVector3TransformNormal(v, m));
}

Vec3 TransformCoord(const Mat& m, const Vec3& v)
{
    return Vec3(XMVector3TransformCoord(v, m));
}

Vec3 TransformVector(const Mat& m, const Vec3& v)
{
    return Vec3(XMVector3TransformNormal(v, m));
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
    XMVECTOR qa = a;
    XMVECTOR qb = b;
    
    if (XMVectorGetX(XMQuaternionDot(qa, qb)) < 0.0f)
    {
        qb = XMVectorNegate(qb);
    }
    return Quat(XMQuaternionSlerp(qa, qb, t));
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

bool Decompose(const Mat& m, Vec3& s, Quat& r, Vec3& t)
{
    XMVECTOR scale, rot, trans;
    if (!XMMatrixDecompose(&scale, &rot, &trans, m))
    {
        return false;
    }
    XMStoreFloat3(&s, scale);
    XMStoreFloat4(&r, rot);    
    XMStoreFloat3(&t, trans);
    return true;
}

float PlaneLength(const Vec4& plane, const Vec3& point)
{
    float l = plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
    return l;
}


float NormalizeAngleDeg(float angle)
{
    angle = std::fmodf(angle, 360.0f);
    if (angle < 0.0f)
    {
        angle += 360.0f;
    }
    return angle;
}

float NormalizeAngleRad(float angle)
{
    angle = std::fmodf(angle,k2PI);
    if (angle < 0.0f)
    {
        angle += k2PI;
    }
    return angle;
}

float NormalizeAngleRadSigned(float angle)
{
    return std::remainder(angle, k2PI);
}

float PowF(float num, int power)
{
    float tmp = num;
    for (int i = 1; i <= power; i++)
    {
        tmp = tmp * num;
    }
    return tmp;
}

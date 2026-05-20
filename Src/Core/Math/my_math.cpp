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

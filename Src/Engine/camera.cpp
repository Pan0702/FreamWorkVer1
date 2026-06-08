#include "camera.h"

void Camera::Initialize(float fov, float aspect, float near_z, float far_z)
{
    fov_ = fov;
    aspect_ = aspect;
    near_z_ = near_z;
    far_z_ = far_z;
    pos_ = Vec3(0.0f, 0.0f, 0.0f);
    up_ = Vec3(0.0f, 1.0f, 0.0f);
}

Mat Camera::GetViewMatrix()
{
    Vec3 look = look_;
    if (Distance(pos_, look) < 1e-4f)
    {
        look = look_ + Vec3(0, 0, 0.01f);
    }
    view_ = LookAtLH(pos_, look_, up_);
    return view_;
}

Mat Camera::GetProjectionMatrix()
{
    IsZeroVec(look_);
    proj_ = PerspectiveFovLH(fov_, aspect_, near_z_, far_z_);
    return proj_;
}

Vec3 Camera::GetForward()
{
    IsZeroVec(look_);
    return look_;
}

Vec3 Camera::GetRight()
{
    IsZeroVec(look_);
    return (Cross(Vec3(0, 1, 0), look_)).Normalized();
}

Vec3 Camera::GetUp() const
{
    return up_;
}

void Camera::SetAspect(float aspect)
{
    aspect_ = aspect;
}

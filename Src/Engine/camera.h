#pragma once
#include "../Core/Math/my_math.h"

class Camera
{
public:
    void Initialize(float fov, float aspect, float near_z, float far_z);
    Mat GetViewMatrix();
    Mat GetProjectionMatrix() ;
    Vec3 GetForward() ;
    Vec3 GetRight() ;
    Vec3 GetUp() const;
    
    Vec3 pos_;
    Vec3 look_;
    Vec3 up_;
    Mat view_;
    Mat proj_;
private:
    float fov_;
    float aspect_;
    float near_z_;
    float far_z_;
    
};

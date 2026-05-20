#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat.h"
Mat Identity();
Mat Transpose(const Mat& m);
Mat Inverse(const Mat& m);
Mat RotateX(float rot);
Mat RotateY(float rot);
Mat RotateZ(float rot);
Mat Scale(const Vec3& scale);
Mat Translate(const Vec3& trans);
Mat LookAtLH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
Mat LookAtRH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);

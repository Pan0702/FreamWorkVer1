#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat.h"
#include "quat.h"
#include <numbers>
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
Mat PerspectiveFovLH(float fov, float aspect, float zNear, float zFar);
Vec3 TransformDirection(const Mat& m, const Vec3& v);
Vec3 TransformPoint(const Mat& m, const Vec3& v);
Vec3 TransformNormal(const Mat& m, const Vec3& v);
Vec3 TransformCoord(const Mat& m, const Vec3& v);
Vec3 TransformVector(const Mat& m, const Vec3& v);

Quat QuatIdentity();
Quat QuatFromAxisAngle(const Vec3& axis, float angle);
Quat QuatFromEuler(const Vec3& euler);
Quat Slerp(const Quat& a, const Quat& b, float t);

Mat ToMat(const Quat& q);
Quat ToQuat(const Mat& m);
Vec3 Rotate(const Quat& q,const Vec3& v);
bool Decompose(const Mat& m, Vec3& s, Quat& r, Vec3& t);

constexpr float kPI = std::numbers::pi_v<float>;
constexpr float k2PI = 2.0f * kPI;
constexpr float kHalfPi = kPI * 0.5f;
constexpr float kDegToRad = kPI / 180.0f;
constexpr float kRadToDeg = 180.0f / kPI;

using uint32 = uint32_t;
using int32 = int32_t;
using uint64 =uint64_t;
using int64 = int64_t;
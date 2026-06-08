#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat.h"
#include "quat.h"
#include <numbers>

/**
 * @brief Identityを行う関数。
 * @return 戻り値。
 */
Mat Identity();
/**
 * @brief Transposeを行う関数。
 * @param m 引数。
 * @return 戻り値。
 */
Mat Transpose(const Mat& m);
/**
 * @brief Inverseを行う関数。
 * @param m 引数。
 * @return 戻り値。
 */
Mat Inverse(const Mat& m);
/**
 * @brief RotateXを行う関数。
 * @param rot 引数。
 * @return 戻り値。
 */
Mat RotateX(float rot);
/**
 * @brief RotateYを行う関数。
 * @param rot 引数。
 * @return 戻り値。
 */
Mat RotateY(float rot);
/**
 * @brief RotateZを行う関数。
 * @param rot 引数。
 * @return 戻り値。
 */
Mat RotateZ(float rot);
/**
 * @brief Scaleを行う関数。
 * @param scale 引数。
 * @return 戻り値。
 */
Mat Scale(const Vec3& scale);
/**
 * @brief Translateを行う関数。
 * @param trans 引数。
 * @return 戻り値。
 */
Mat Translate(const Vec3& trans);
/**
 * @brief LookAtLHを行う関数。
 * @param eye 引数。
 * @param at 引数。
 * @param up 引数。
 * @return 戻り値。
 */
Mat LookAtLH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
/**
 * @brief LookAtRHを行う関数。
 * @param eye 引数。
 * @param at 引数。
 * @param up 引数。
 * @return 戻り値。
 */
Mat LookAtRH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
/**
 * @brief PerspectiveFovLHを行う関数。
 * @param fov 引数。
 * @param aspect 引数。
 * @param zNear 引数。
 * @param zFar 引数。
 * @return 戻り値。
 */
Mat PerspectiveFovLH(float fov, float aspect, float zNear, float zFar);
/**
 * @brief TransformDirectionを行う関数。
 * @param m 引数。
 * @param v 引数。
 * @return 戻り値。
 */
Vec3 TransformDirection(const Mat& m, const Vec3& v);
/**
 * @brief TransformPointを行う関数。
 * @param m 引数。
 * @param v 引数。
 * @return 戻り値。
 */
Vec3 TransformPoint(const Mat& m, const Vec3& v);
/**
 * @brief TransformNormalを行う関数。
 * @param m 引数。
 * @param v 引数。
 * @return 戻り値。
 */
Vec3 TransformNormal(const Mat& m, const Vec3& v);
/**
 * @brief TransformCoordを行う関数。
 * @param m 引数。
 * @param v 引数。
 * @return 戻り値。
 */
Vec3 TransformCoord(const Mat& m, const Vec3& v);
/**
 * @brief TransformVectorを行う関数。
 * @param m 引数。
 * @param v 引数。
 * @return 戻り値。
 */
Vec3 TransformVector(const Mat& m, const Vec3& v);

/**
 * @brief QuatIdentityを行う関数。
 * @return 戻り値。
 */
Quat QuatIdentity();
/**
 * @brief QuatFromAxisAngleを行う関数。
 * @param axis 引数。
 * @param angle 引数。
 * @return 戻り値。
 */
Quat QuatFromAxisAngle(const Vec3& axis, float angle);
/**
 * @brief QuatFromEulerを行う関数。
 * @param euler 引数。
 * @return 戻り値。
 */
Quat QuatFromEuler(const Vec3& euler);
/**
 * @brief Slerpを行う関数。
 * @param a 引数。
 * @param b 引数。
 * @param t 引数。
 * @return 戻り値。
 */
Quat Slerp(const Quat& a, const Quat& b, float t);

/**
 * @brief ToMatを行う関数。
 * @param q 引数。
 * @return 戻り値。
 */
Mat ToMat(const Quat& q);
/**
 * @brief ToQuatを行う関数。
 * @param m 引数。
 * @return 戻り値。
 */
Quat ToQuat(const Mat& m);
/**
 * @brief Rotateを行う関数。
 * @param q 引数。
 * @param v 引数。
 * @return 戻り値。
 */
Vec3 Rotate(const Quat& q, const Vec3& v);
/**
 * @brief Decomposeを行う関数。
 * @param m 引数。
 * @param s 引数。
 * @param r 引数。
 * @param t 引数。
 * @return 条件を満たす場合は true。
 */
bool Decompose(const Mat& m, Vec3& s, Quat& r, Vec3& t);

// 角度変換と円周率の定数。
constexpr float kPI = std::numbers::pi_v<float>;
constexpr float k2PI = 2.0f * kPI;
constexpr float kHalfPi = kPI * 0.5f;
constexpr float kDegToRad = kPI / 180.0f;
constexpr float kRadToDeg = 180.0f / kPI;

// よく使う整数型の短縮別名。
using uint32 = uint32_t;
using int32 = int32_t;
using uint64 = uint64_t;
using int64 = int64_t;

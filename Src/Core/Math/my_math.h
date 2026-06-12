#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat.h"
#include "quat.h"
#include <numbers>

/**
 * @brief 単位行列を作成する。
 * @return 単位行列。
 */
Mat Identity();
/**
 * @brief 行列を転置する。
 * @param m 転置する行列。
 * @return 転置後の行列。
 */
Mat Transpose(const Mat& m);
/**
 * @brief 行列の逆行列を作成する。
 * @param m 逆行列を求める行列。
 * @return 逆行列。
 */
Mat Inverse(const Mat& m);
/**
 * @brief X軸回転行列を作成する。
 * @param rot 回転角度。単位はラジアン。
 * @return X軸回転行列。
 */
Mat RotateX(float rot);
/**
 * @brief Y軸回転行列を作成する。
 * @param rot 回転角度。単位はラジアン。
 * @return Y軸回転行列。
 */
Mat RotateY(float rot);
/**
 * @brief Z軸回転行列を作成する。
 * @param rot 回転角度。単位はラジアン。
 * @return Z軸回転行列。
 */
Mat RotateZ(float rot);
/**
 * @brief 拡大縮小行列を作成する。
 * @param scale 各軸の拡大率。
 * @return 拡大縮小行列。
 */
Mat Scale(const Vec3& scale);
/**
 * @brief 平行移動行列を作成する。
 * @param trans 移動量。
 * @return 平行移動行列。
 */
Mat Translate(const Vec3& trans);
/**
 * @brief 左手座標系のビュー行列を作成する。
 * @param eye カメラ位置。
 * @param at 注視点。
 * @param up カメラの上方向。
 * @return 左手座標系のビュー行列。
 */
Mat LookAtLH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
/**
 * @brief 右手座標系のビュー行列を作成する。
 * @param eye カメラ位置。
 * @param at 注視点。
 * @param up カメラの上方向。
 * @return 右手座標系のビュー行列。
 */
Mat LookAtRH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
/**
 * @brief 左手座標系の透視投影行列を作成する。
 * @param fov 垂直方向の視野角。単位はラジアン。
 * @param aspect アスペクト比。
 * @param zNear ニアクリップ面までの距離。
 * @param zFar ファークリップ面までの距離。
 * @return 左手座標系の透視投影行列。
 */
Mat PerspectiveFovLH(float fov, float aspect, float zNear, float zFar);
/**
 * @brief ベクトルを方向として行列で変換する。
 * @param m 変換に使う行列。
 * @param v 変換する方向ベクトル。
 * @return 変換後の方向ベクトル。
 */
Vec3 TransformDirection(const Mat& m, const Vec3& v);
/**
 * @brief ベクトルを座標点として行列で変換する。
 * @param m 変換に使う行列。
 * @param v 変換する座標点。
 * @return 変換後の座標点。
 */
Vec3 TransformPoint(const Mat& m, const Vec3& v);
/**
 * @brief 法線ベクトルを行列で変換する。
 * @param m 変換に使う行列。
 * @param v 変換する法線ベクトル。
 * @return 変換後の法線ベクトル。
 */
Vec3 TransformNormal(const Mat& m, const Vec3& v);
/**
 * @brief 座標ベクトルを行列で変換する。
 * @param m 変換に使う行列。
 * @param v 変換する座標ベクトル。
 * @return 変換後の座標ベクトル。
 */
Vec3 TransformCoord(const Mat& m, const Vec3& v);
/**
 * @brief ベクトルを行列で変換する。
 * @param m 変換に使う行列。
 * @param v 変換するベクトル。
 * @return 変換後のベクトル。
 */
Vec3 TransformVector(const Mat& m, const Vec3& v);

/**
 * @brief 単位クォータニオンを作成する。
 * @return 単位クォータニオン。
 */
Quat QuatIdentity();
/**
 * @brief 回転軸と角度からクォータニオンを作成する。
 * @param axis 回転軸。
 * @param angle 回転角度。単位はラジアン。
 * @return 回転を表すクォータニオン。
 */
Quat QuatFromAxisAngle(const Vec3& axis, float angle);
/**
 * @brief オイラー角からクォータニオンを作成する。
 * @param euler 各軸の回転角度。単位はラジアン。
 * @return 回転を表すクォータニオン。
 */
Quat QuatFromEuler(const Vec3& euler);
/**
 * @brief 2つのクォータニオンを球面線形補間する。
 * @param a 補間開始のクォータニオン。
 * @param b 補間終了のクォータニオン。
 * @param t 補間率。0.0でa、1.0でb。
 * @return 補間後のクォータニオン。
 */
Quat Slerp(const Quat& a, const Quat& b, float t);

/**
 * @brief クォータニオンを回転行列に変換する。
 * @param q 変換するクォータニオン。
 * @return 回転行列。
 */
Mat ToMat(const Quat& q);
/**
 * @brief 回転行列をクォータニオンに変換する。
 * @param m 変換する回転行列。
 * @return クォータニオン。
 */
Quat ToQuat(const Mat& m);
/**
 * @brief クォータニオンでベクトルを回転する。
 * @param q 回転を表すクォータニオン。
 * @param v 回転するベクトル。
 * @return 回転後のベクトル。
 */
Vec3 Rotate(const Quat& q, const Vec3& v);
/**
 * @brief 行列を拡大縮小、回転、平行移動に分解する。
 * @param m 分解する行列。
 * @param s 分解した拡大縮小を受け取る変数。
 * @param r 分解した回転を受け取る変数。
 * @param t 分解した平行移動を受け取る変数。
 * @return 分解に成功した場合は true。
 */
bool Decompose(const Mat& m, Vec3& s, Quat& r, Vec3& t);

float PlaneLength(const Vec4& plane,const Vec3& point);

// 角度変換と円周率の定数。
constexpr float kPI = std::numbers::pi_v<float>;
constexpr float k2PI = 2.0f * kPI;
constexpr float kHalfPi = kPI * 0.5f;
constexpr float kDegToRad = kPI / 180.0f;
constexpr float kRadToDeg = 180.0f / kPI;
constexpr float kHalfSize = 0.5f;

// よく使う整数型の短縮別名。
using uint32 = uint32_t;
using int32 = int32_t;
using uint64 = uint64_t;
using int64 = int64_t;
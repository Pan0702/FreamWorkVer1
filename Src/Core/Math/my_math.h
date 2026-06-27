#pragma once
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat.h"
#include "quat.h"
#include <numbers>

/**
 * @brief 単位行列を作成する。
 * @return 対角成分が 1 の単位行列。
 */
Mat Identity();
/**
 * @brief 行列の行と列を入れ替える。
 * @param m 計算に使用する行列または値。
 * @return m の行と列を入れ替えた行列。
 */
Mat Transpose(const Mat& m);
/**
 * @brief 逆行列を作成する。
 * @param m 計算に使用する行列または値。
 * @return m の逆行列。
 */
Mat Inverse(const Mat& m);
/**
 * @brief X 軸まわりの回転行列を作成する。
 * @param rot 回転角度。
 * @return X 軸まわりに angle だけ回転する行列。
 */
Mat RotateX(float rot);
/**
 * @brief Y 軸まわりの回転行列を作成する。
 * @param rot 回転角度。
 * @return Y 軸まわりに angle だけ回転する行列。
 */
Mat RotateY(float rot);
/**
 * @brief Z 軸まわりの回転行列を作成する。
 * @param rot 回転角度。
 * @return Z 軸まわりに angle だけ回転する行列。
 */
Mat RotateZ(float rot);
/**
 * @brief 拡大縮小行列を作成する。
 * @param scale 拡大縮小量。
 * @return scale を反映した拡大縮小行列。
 */
Mat Scale(const Vec3& scale);
/**
 * @brief 平行移動行列を作成する。
 * @param trans 平行移動量。
 * @return trans を反映した平行移動行列。
 */
Mat Translate(const Vec3& trans);
/**
 * @brief 左手座標系のビュー行列を作成する。
 * @param eye カメラ位置。
 * @param at 注視点。
 * @param up カメラの上方向。
 * @return 左手座標系で eye から at を見るビュー行列。
 */
Mat LookAtLH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
/**
 * @brief 右手座標系のビュー行列を作成する。
 * @param eye カメラ位置。
 * @param at 注視点。
 * @param up カメラの上方向。
 * @return 右手座標系で eye から at を見るビュー行列。
 */
Mat LookAtRH(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up);
/**
 * @brief 左手座標系の透視投影行列を作成する。
 * @param fov 縦方向の視野角。
 * @param aspect 画面のアスペクト比。
 * @param zNear ニアクリップ面の距離。
 * @param zFar ファークリップ面の距離。
 * @return 左手座標系の透視投影行列。
 */
Mat PerspectiveFovLH(float fov, float aspect, float zNear, float zFar);
/**
 * @brief 方向ベクトルを行列で変換する。
 * @param m 計算に使用する行列または値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 行列で変換した方向ベクトル。
 */
Vec3 TransformDirection(const Mat& m, const Vec3& v);
/**
 * @brief 座標点を行列で変換する。
 * @param m 計算に使用する行列または値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 行列で変換した座標点。
 */
Vec3 TransformPoint(const Mat& m, const Vec3& v);
/**
 * @brief 法線ベクトルを行列で変換する。
 * @param m 計算に使用する行列または値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 行列で変換した法線ベクトル。
 */
Vec3 TransformNormal(const Mat& m, const Vec3& v);
/**
 * @brief 座標ベクトルを同次除算込みで変換する。
 * @param m 計算に使用する行列または値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 同次除算まで行った変換後の座標。
 */
Vec3 TransformCoord(const Mat& m, const Vec3& v);
/**
 * @brief ベクトルを行列で変換する。
 * @param m 計算に使用する行列または値。
 * @param v 計算に使用するベクトルまたは点。
 * @return 行列で変換したベクトル。
 */
Vec3 TransformVector(const Mat& m, const Vec3& v);

/**
 * @brief 回転なしを表す単位クォータニオンを作成する。
 * @return 回転なしを表す単位クォータニオン。
 */
Quat QuatIdentity();
/**
 * @brief 軸と角度から回転クォータニオンを作成する。
 * @param axis 回転軸。
 * @param angle 回転角度。
 * @return axis と angle から作成した回転クォータニオン。
 */
Quat QuatFromAxisAngle(const Vec3& axis, float angle);
/**
 * @brief オイラー角から回転クォータニオンを作成する。
 * @param euler euler に設定する値。
 * @return euler から作成した回転クォータニオン。
 */
Quat QuatFromEuler(const Vec3& euler);
/**
 * @brief 2 つの回転を球面線形補間する。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param t 補間率。
 * @return a から b へ t だけ球面補間したクォータニオン。
 */
Quat Slerp(const Quat& a, const Quat& b, float t);

/**
 * @brief クォータニオンを回転行列へ変換する。
 * @param q q に設定する値。
 * @return q を変換した回転行列。
 */
Mat ToMat(const Quat& q);
/**
 * @brief 行列から回転クォータニオンを取り出す。
 * @param m 計算に使用する行列または値。
 * @return m から取り出した回転クォータニオン。
 */
Quat ToQuat(const Mat& m);
/**
 * @brief ベクトルへクォータニオン回転を適用する。
 * @param q q に設定する値。
 * @param v 計算に使用するベクトルまたは点。
 * @return q で回転させた v。
 */
Vec3 Rotate(const Quat& q, const Vec3& v);
/**
 * @brief 行列をスケール・回転・平行移動へ分解する。
 * @param m 計算に使用する行列または値。
 * @param s s に設定する値。
 * @param r r に設定する値。
 * @param t 補間率。
 * @return 行列を scale・rotation・translation に分解できた場合は true。
 */
bool Decompose(const Mat& m, Vec3& s, Quat& r, Vec3& t);

/**
 * @brief 数学計算の結果を求める。
 * @param plane plane に設定する値。
 * @param point 計算に使用するベクトルまたは点。
 * @return plane から point までの符号付き距離。
 */
float PlaneLength(const Vec4& plane,const Vec3& point);
/**
 * @brief 数学計算の結果を求める。
 * @param angle 回転角度。
 * @return 0 以上 360 未満に正規化した角度。
 */
float NormalizeAngleDeg(float angle);
/**
 * @brief 数学計算の結果を求める。
 * @param angle 回転角度。
 * @return 0 以上 2π 未満に正規化した角度。
 */
float NormalizeAngleRad(float angle);
/**
 * @brief 数学計算の結果を求める。
 * @param angle 回転角度。
 * @return -π 以上 π 未満に正規化した角度。
 */
float NormalizeAngleRadSigned(float angle);
/**
 * @brief 数学計算の結果を求める。
 * @param num num に設定する値。
 * @param power power に設定する値。
 * @return num を power 回掛け合わせた値。
 */
float PowF(float num,int power);

// 角度変換と円周率の定数。
constexpr float kPI = std::numbers::pi_v<float>;
constexpr float k2PI = 2.0f * kPI;
constexpr float kHalfPi = kPI * 0.5f;
constexpr float kDegToRad = kPI / 180.0f;
constexpr float kRadToDeg = 180.0f / kPI;
constexpr float kHalfSize = 0.5f;


// よく使う整数型の短縮別名。
using uint8 = uint8_t;
using int8 = int8_t;
using uint16 = uint16_t;
using int16 = int16_t;
using uint32 = uint32_t;
using int32 = int32_t;
using uint64 = uint64_t;
using int64 = int64_t;

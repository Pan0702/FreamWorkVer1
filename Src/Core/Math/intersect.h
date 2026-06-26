#pragma once
#include "my_math.h"

// Box に関係する状態と振る舞いをまとめる型。
struct Box
{
    Vec3 min;
    Vec3 max;
};

// Sphere に関係する状態と振る舞いをまとめる型。
struct Sphere
{
    Vec3 center;
    float radius;
};

// ContactInfo に関係する状態と振る舞いをまとめる型。
struct ContactInfo
{
    Vec3  normal;  // 押し出し方向（単位ベクトル）
    float depth;   // 重なっている量（プラスの値）
};

// Ray に関係する状態と振る舞いをまとめる型。
struct Ray
{
    Vec3 origin;
    Vec3 direction;
    float distance;
};

struct Capsule
{
    Vec3 center;
    Vec3 dir;
    float radius;
    float height;
};
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param s1 s1 に設定する値。
 * @param s2 s2 に設定する値。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
bool Contact(const Sphere& s1, const Sphere& s2, ContactInfo& out);
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param s s に設定する値。
 * @param box box に設定する値。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
bool Contact(const Sphere& s, const Box& box, ContactInfo& out);
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param b1 b1 に設定する値。
 * @param b2 b2 に設定する値。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
bool Contact(const Box& b1, const Box& b2, ContactInfo& out);
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param s s に設定する値。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param c 計算に使用するベクトルまたは点。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
bool Contact(const Sphere& s, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param box box に設定する値。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param c 計算に使用するベクトルまたは点。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
bool Contact(const Box& box, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);
/**
 * @brief 形状同士の判定を行い、必要な接触情報を組み立てる。
 * @param ray 判定に使用するレイ。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param c 計算に使用するベクトルまたは点。
 * @param out 計算結果を書き込む情報。
 * @return 形状同士の判定を行い、必要な接触情報を組み立てる 場合は true。
 */
bool Contact(const Ray& ray, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

bool Contact(const Capsule& capsule,const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

/**
 * @brief 点から AABB 上の最近接点を求める。
 * @param p 計算に使用するベクトルまたは点。
 * @param box box に設定する値。
 * @return AABB 上で p に最も近い点。
 */
Vec3 ClosestPointOnBox(const Vec3& p, const Box& box);
/**
 * @brief 点から三角形上の最近接点を求める。
 * @param p 計算に使用するベクトルまたは点。
 * @param a 計算に使用するベクトルまたは点。
 * @param b 計算に使用するベクトルまたは点。
 * @param c 計算に使用するベクトルまたは点。
 * @return 三角形上で point に最も近い点。
 */
Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);

bool  Intersect(const Vec3& capsule_pos, const Vec3& a, const Vec3& b, float radius, Vec3& out);
float SegmentToSegment(const Vec3& start, const Vec3& end, const Vec3& tri_start, const Vec3& tri_end, Vec3& closest1,
                       Vec3& closest2);
bool IsInTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);
bool PointToSurface(const Vec3& end, const Vec3& a, const Vec3& b, const Vec3& c, Vec3& q);

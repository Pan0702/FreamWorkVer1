#pragma once
#include "my_math.h"

// AABB の最小点と最大点を保持する箱。//
struct Box
{
    Vec3 min;
    Vec3 max;
};

// 球の中心座標と半径を保持する形状。//
struct Sphere
{
    Vec3 center;
    float radius;
};

// 接触時の押し出し方向と貫通量を受け取る情報。//
struct ContactInfo
{
    Vec3  normal;  // 第1引数の形状を押し出す向き。//
    float depth;   // 重なっている量。//
};

// 始点、向き、最大距離を持つレイ判定用の形状。//
struct Ray
{
    Vec3 origin;
    Vec3 direction;
    float distance;
};

// 中心、軸方向、半径、高さを持つカプセル形状。//
struct Capsule
{
    Vec3 center;
    Vec3 dir;
    float radius;
    float height;
};

// 3 頂点で表す三角形。//
struct Triangle
{
    Vec3 a;
    Vec3 b;
    Vec3 c;
};

// AABB の最小点と最大点を保持する境界箱。//
struct AABB
{
    Vec3 min;
    Vec3 max;
};

/**
 * @brief 球同士の当たり判定を行う。
 * @param s1 押し出し対象になる球。
 * @param s2 判定相手になる球。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return 2 つの球が重なっている場合は true。
 */
bool Contact(const Sphere& s1, const Sphere& s2, ContactInfo& out);

/**
 * @brief 球と AABB の当たり判定を行う。
 * @param s 押し出し対象になる球。
 * @param box 判定相手になる AABB。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return 球と AABB が重なっている場合は true。
 */
bool Contact(const Sphere& s, const Box& box, ContactInfo& out);

/**
 * @brief AABB 同士の当たり判定を行う。
 * @param b1 押し出し対象になる AABB。
 * @param b2 判定相手になる AABB。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return 2 つの AABB が重なっている場合は true。
 */
bool Contact(const Box& b1, const Box& b2, ContactInfo& out);

/**
 * @brief 球と三角形の当たり判定を行う。
 * @param s 押し出し対象になる球。
 * @param a 三角形の頂点 A。
 * @param b 三角形の頂点 B。
 * @param c 三角形の頂点 C。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return 球と三角形が重なっている場合は true。
 */
bool Contact(const Sphere& s, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

/**
 * @brief AABB と三角形の当たり判定を行う。
 * @param box 押し出し対象になる AABB。
 * @param a 三角形の頂点 A。
 * @param b 三角形の頂点 B。
 * @param c 三角形の頂点 C。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return AABB と三角形が重なっている場合は true。
 */
bool Contact(const Box& box, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

/**
 * @brief レイと三角形の交差判定を行う。
 * @param ray 判定に使用するレイ。
 * @param a 三角形の頂点 A。
 * @param b 三角形の頂点 B。
 * @param c 三角形の頂点 C。
 * @param out ヒット距離と面法線を書き込む情報。
 * @return レイが距離範囲内で三角形に当たった場合は true。
 */
bool Contact(const Ray& ray, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

/**
 * @brief カプセルと三角形の当たり判定を行う。
 * @param capsule 押し出し対象になるカプセル。
 * @param a 三角形の頂点 A。
 * @param b 三角形の頂点 B。
 * @param c 三角形の頂点 C。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return カプセルと三角形が重なっている場合は true。
 */
bool Contact(const Capsule& capsule, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

/**
 * @brief 点から AABB 上の最近接点を求める。
 * @param p AABB に近づけたい点。
 * @param box 最近接点を求める AABB。
 * @return AABB 上で p に最も近い点。
 */
Vec3 ClosestPointOnBox(const Vec3& p, const Box& box);

/**
 * @brief 点から三角形上の最近接点を求める。
 * @param p 三角形に近づけたい点。
 * @param a 三角形の頂点 A。
 * @param b 三角形の頂点 B。
 * @param c 三角形の頂点 C。
 * @return 三角形上で p に最も近い点。
 */
Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);

/**
 * @brief 点と線分カプセルの当たり判定を行う。
 * @param capsule_pos カプセル側の点。
 * @param a 線分の始点。
 * @param b 線分の終点。
 * @param radius カプセル半径。
 * @param out 線分上の最近接点を書き込む値。
 * @return 点が線分から radius 以内にある場合は true。
 */
bool Intersect(const Vec3& capsule_pos, const Vec3& a, const Vec3& b, float radius, Vec3& out);

/**
 * @brief カプセル線分と三角形の辺の接触を調べる。
 * @param start カプセル軸線分の始点。
 * @param end カプセル軸線分の終点。
 * @param t 判定相手になる三角形。
 * @param r カプセル半径。
 * @param out 接触法線と貫通量を書き込む情報。
 * @return カプセル軸線分が三角形の辺に r 以内で近づいた場合は true。
 */
bool SegmentIntersect(const Vec3& start, const Vec3& end, const Triangle& t, float r, ContactInfo& out);

/**
 * @brief 2 つの線分間の最近接点と距離の二乗を求める。
 * @param start 1 本目の線分の始点。
 * @param end 1 本目の線分の終点。
 * @param tri_start 2 本目の線分の始点。
 * @param tri_end 2 本目の線分の終点。
 * @param closest1 1 本目の線分上の最近接点を書き込む値。
 * @param closest2 2 本目の線分上の最近接点を書き込む値。
 * @return 2 つの最近接点の距離の二乗。
 */
float SegmentToSegment(const Vec3& start, const Vec3& end, const Vec3& tri_start, const Vec3& tri_end,
                       Vec3& closest1, Vec3& closest2);

/**
 * @brief 点が三角形の内側にあるかを重心座標で調べる。
 * @param p 判定したい点。
 * @param t 判定相手になる三角形。
 * @return p が三角形の内側または辺上にある場合は true。
 */
bool IsInTriangle(const Vec3& p, const Triangle& t);

/**
 * @brief 点を三角形平面へ射影し、半径内で面に届くか調べる。
 * @param end 判定したいカプセル端点。
 * @param t 判定相手になる三角形。
 * @param r カプセル半径。
 * @param q 三角形平面上の射影点を書き込む値。
 * @return 射影点が三角形内にあり、点から平面までの距離が r 以下の場合は true。
 */
bool PointToSurface(const Vec3& end, const Triangle& t, float r, Vec3& q);
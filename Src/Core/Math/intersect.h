#pragma once
#include "my_math.h"

struct Box
{
    Vec3 min;
    Vec3 max;
};

struct Sphere
{
    Vec3 center;
    float radius;
};

struct ContactInfo
{
    Vec3  normal;  // 押し出し方向（単位ベクトル）
    float depth;   // 重なっている量（プラスの値）
};

// 接触判定: out.normal は第1引数を第2引数から押し出す向き、out.depth は正の貫通量
bool Contact(const Sphere& s1, const Sphere& s2, ContactInfo& out);
bool Contact(const Sphere& s, const Box& box, ContactInfo& out);
bool Contact(const Box& b1, const Box& b2, ContactInfo& out);
bool Contact(const Sphere& s, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);
bool Contact(const Box& box, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);

// 最近接点
Vec3 ClosestPointOnBox(const Vec3& p, const Box& box);
Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);
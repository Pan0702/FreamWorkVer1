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
    Vec3  normal;  // 押し出す向き（単位ベクトル）
    float depth;   // 重なってる量（プラスの値）
};
Vec4 PlaneSet(const Vec3& p0, const Vec3& p1,const Vec3& p2);
Vec4 PlaneSet(const Vec3& normal, const Vec3& point);
bool Intersect(const Sphere& sphere1, const Sphere& sphere2);
bool Intersect(const Sphere& sphere, const Box& box);
bool Intersect(const Box& box1, const Box& box2);
bool Intersect(const Sphere& s,const Vec3& a, const Vec3& b,const Vec3& c);
bool Intersect(const Box& box,const Vec3& a, const Vec3& b,const Vec3& c);

bool HitCheckPlaneSphere(Vec4* hit_vec,const Vec4& plane, const Sphere& sphere);
bool HitCheckTriangleSphere(Vec4* hit_vec, const Vec3& a, const Vec3& b, const Vec3& c, const Sphere& sphere);
bool HitCheckSphereSegment(Vec4* hit_vec, const Sphere& sphere, const Vec3& a, const Vec3& b);
bool HitCheckTriangleSegment(Vec4* hit_pos, const Vec3& a, const Vec3& b, const Vec3& c,const Vec3& seg,const Vec3& pos);
bool TriangleCheckInner(const Vec3& a,const Vec3& b,const Vec3& c,const Vec3& p);
bool HitCheckPlaneSengment(Vec3* hit_pos, const Vec4& plane, const Vec3& seg,const Vec3& pos);

bool HitCheckAxis(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& axis,const Vec3& half_size);

bool Contact(const Sphere& s1, const Sphere& s2, ContactInfo& out);
bool Contact(const Sphere& s, const Box& box, ContactInfo& out);
bool Contact(const Sphere& s, const Vec3& a, const Vec3& b, const Vec3& c, ContactInfo& out);
Vec3 ClosestPointOnBox(const Vec3& p, const Box& box);
Vec3 ClosestPointOnTriangle(const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c);
#include "intersect.h"

#include <algorithm>

Vec4 PlaneSet(const Vec3& p0, const Vec3& p1, const Vec3& p2)
{
    const Vec3 vec_a = p1 - p0;
    const Vec3 vec_b = p2 - p0;
    //法線を求める
    const Vec3 cross = Cross(vec_a, vec_b);
    return PlaneSet(cross, p0);
}

Vec4 PlaneSet(const Vec3& normal, const Vec3& point)
{
    //正規化
    const Vec3 norm = normal.Normalized();
    Vec4 result;
    result.x = norm.x;
    result.y = norm.y;
    result.z = norm.z;
    result.w = -Dot(norm, point);
    return result;
}

bool Intersect(const Sphere& sphere1, const Sphere& sphere2)
{
    const float r = sphere1.radius + sphere2.radius;
    return (DistanceSquared(sphere1.center, sphere2.center) <= r * r);
}

bool Intersect(const Sphere& sphere, const Box& box)
{
    Vec3 closest;
    closest.x = std::clamp(sphere.center.x, box.min.x, box.max.x);
    closest.y = std::clamp(sphere.center.y, box.min.y, box.max.y);
    closest.z = std::clamp(sphere.center.z, box.min.z, box.max.z);
    return DistanceSquared(closest, sphere.center) <= sphere.radius * sphere.radius;
}

bool Intersect(const Box& box1, const Box& box2)
{
    if (box1.max.x < box2.min.x || box1.min.x > box2.max.x) return false;
    if (box1.max.y < box2.min.y || box1.min.y > box2.max.y) return false;
    if (box1.max.z < box2.min.z || box1.min.z > box2.max.z) return false;
    return true;
}

bool Intersect(const Sphere& s, const Vec3& a, const Vec3& b, const Vec3& c)
{
    Vec4 plane = PlaneSet(a, b, c);
    if (!HitCheckPlaneSphere(nullptr, plane, s))
    {
        return false;
    }
    if (HitCheckSphereSegment(nullptr, s, a, b))
    {
        return true;
    }
    if (HitCheckSphereSegment(nullptr, s, b, c))
    {
        return true;
    }
    if (HitCheckSphereSegment(nullptr, s, c, a))
    {
        return true;
    }
     
    //球が三角形の中心にめり込むか？
    Vec3 vec = Vec3(-plane.x * s.radius, -plane.y * s.radius, -plane.z * s.radius);

    return HitCheckTriangleSegment(nullptr, a, b, c, vec, s.center);
}

bool Intersect(const Box& box, const Vec3& a, const Vec3& b, const Vec3& c)
{
    Vec3 box_center = (box.min + box.max) * 0.5f;
    Vec3 box_half_size = (box.max - box.min) * 0.5f;
    
    Vec3 axes[13];

    Vec3 ta = a - box_center;
    Vec3 tb = b - box_center;
    Vec3 tc = c - box_center;
    
    axes[0] = Vec3(1, 0, 0);
    axes[1] = Vec3(0, 1, 0);
    axes[2] = Vec3(0, 0, 1);

    Vec3 e0 = tb - ta;
    Vec3 e1 = tc - tb;
    Vec3 e2 = ta - tc;
    axes[3] = Cross(e1, e0);
    
    int index = 4;
    
    for (int i = 0; i < 3; i++)
    {
        axes[index++] = Cross(axes[i], e0);
        axes[index++] = Cross(axes[i], e1);
        axes[index++] = Cross(axes[i], e2);
    }

    for (auto axe : axes)
    {
        if (!HitCheckAxis( ta, tb, tc, axe,box_half_size))
        {
            return false;
        }
    }
    return true;
}



bool HitCheckPlaneSphere(Vec4* hit_vec, const Vec4& plane, const Sphere& sphere)
{
    //平面と円の中心距離をくらべる
    float l = PlaneLength(plane, sphere.center);
    if (std::abs(l) > sphere.radius)
    {
        return false;
    }

    if (hit_vec)
    {
        //めり込んだ量
        l = sphere.radius - l;
        *hit_vec = plane * l;
    }
    return true;
}

bool HitCheckSphereSegment(Vec4* hit_vec, const Sphere& sphere, const Vec3& a, const Vec3& b)
{
    const Vec3 dir = b - a;
    const Vec3 seg = a - sphere.center;
    const float c = Dot(dir, seg) * 2.0f;
    float d = c * c - 4.0f * Dot(dir, dir) * (Dot(seg, seg) - sphere.radius * sphere.radius);
    if (d < 0)
    {
        return false;
    }
    d = sqrtf(d);
    const float t0 = (-c + d) / (2.0f * Dot(dir, dir));
    const float t1 = (-c - d) / (2.0f * Dot(dir, dir));
    float t = 2.0f;
    if ((t0 >= 0.0f) && (t0 <= 1.0f) && (t0 < t))
    {
        t = t0;
    }
    if ((t1 >= 0.0f) && (t1 <= 1.0f) && (t1 < t))
    {
        t = t1;
    }

    if (t > 1.0f)  
    {
        return false;
    }

    if (hit_vec)
    {
        *hit_vec = a + (dir * t);
    }
    return true;
}
bool HitCheckTriangleSegment(Vec4* hit_pos, const Vec3& a, const Vec3& b, const Vec3& c,
    const Vec3& seg,const Vec3& pos)
{
    Vec4 plane = PlaneSet(a, b, c);
    Vec3 hit_tmp;
    //三角形を含む平面と線分で確認
    if (!HitCheckPlaneSengment(&hit_tmp,plane,seg,pos))
    {
        return false;
    }
    
    // 交点が三角形内部に存在するか確認する
    if (!TriangleCheckInner(a,b,c,hit_tmp))
    {
        return false;
    }else
    {
        if (hit_pos)
        {
            *hit_pos = Vec4(hit_tmp.x, hit_tmp.y, hit_tmp.z, 1.0f);
        }
        return true;
    }
}

bool TriangleCheckInner(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& p)
{
    //任意の点から拡張店へのベクトル作成
    Vec3 vt0 = a - p;
    Vec3 vt1 = b - p;
    Vec3 vt2 = c - p;
    
    // 三角形の辺をなぞるベクトルを作成
    Vec3 v0 = b - a;
    Vec3 v1 = c - b;
    Vec3 v2 = a - c;
    
    // 各外戚を求める
    Vec3 c0 = Cross(vt0,v0);
    Vec3 c1 = Cross(vt1,v1);
    Vec3 c2 = Cross(vt2,v2);
    
    // 外積の向きがそろっているかチェック
    float dot0 = Dot(c0, c1);
    float dot1 = Dot(c1, c2);
    float dot2 = Dot(c2, c0);
    return (dot0 >= 0.0f && dot1 >= 0.0f && dot2 >= 0.0f);
}

bool HitCheckPlaneSengment(Vec3* hit_pos, const Vec4& plane, const Vec3& seg, const Vec3& pos)
{
    float l = PlaneLength(plane, pos);
    float l2 = seg.LengthSquared();
    if (l2 < (l * l))
    {
        return false;
    }
    float vec_l = sqrtf(l2);
    //
    Vec4 Vec = Vec4(seg * (1.0f / vec_l));
    // 線分ベクトルを正規化
    float t = Dot(-plane, Vec);
    
    float pln_l = l / t;
    if (pln_l > vec_l)
    {
        return false;
    }
    if (hit_pos)
    {
        *hit_pos = pos + Vec * pln_l;
    }
    return true;
}

bool HitCheckAxis(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& axis, const Vec3& half_size)
{
    if (axis.LengthSquared() < 1e-8f)
    {
        return true;
    }
    
    float t = Dot(axis, a);
    float t1 = Dot(axis, b);
    float t2 = Dot(axis, c);
    
    float tri_max = (std::max)({ t, t1, t2 });
    float tri_min = (std::min)({ t, t1, t2 });
    
    float r = half_size.x *  std::abs(axis.x) + 
        half_size.y *  std::abs(axis.y) + 
        half_size.z *  std::abs(axis.z);
    
    if (tri_max < -r || tri_min > r)
    {
        return false;
    }
    return true;
    
}
